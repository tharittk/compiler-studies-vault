#define DEBUG_TYPE "cfg-profiler"
#include "BBProfiler.h"
#include "ProfileUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
// #include "llvm/Support/Alignment.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include <iostream>

using namespace llvm;
using namespace fun;

STATISTIC(numBBs, "The # of BBs profiled.");

char BBProfiler::ID = 0;
static RegisterPass<BBProfiler> rp("bb-profiler", "Basic Block Profiler");

static cl::opt<std::string>
    outputFileName("bb-info-output-file", cl::init("bb_info.prof"),
                   cl::NotHidden, cl::Optional, cl::ValueRequired,
                   cl::value_desc("filename"),
                   cl::desc("Output profile info file (used by -bb-profiler)"));

GlobalVariable *CreateGlobalCounter(Module &m, StringRef VarName) {
  auto &ctxt = m.getContext();
  // declaration
  Constant *NewGlobalVar =
      m.getOrInsertGlobal(VarName, IntegerType::getInt32Ty(ctxt));
  // definition
  GlobalVariable *NewGV = m.getNamedGlobal(VarName);
  NewGV->setLinkage(GlobalValue::CommonLinkage);
  // NewGV->setAlignment(4);
  NewGV->setInitializer(ConstantInt::get(ctxt, APInt(32, 0)));
  return NewGV;
}

bool BBProfiler::runOnModule(Module &m) {
  auto &ctxt = m.getContext();

  std::cout << "Run BB Profile ! \n";

  IRBuilder<> Builder(ctxt);
  // STEP 1: Code Injection
  for (auto &F : m) {
    int BBNum = 0; // BB numbering
    for (auto &BB : F) {
      // Create global counter for each basic block
      // This must be a constant address because we basically
      // encode that address info for code generation
      std::string BBName =
          F.getName().str() + std::string("_BB_") + std::to_string(BBNum);

      GlobalVariable *Var = CreateGlobalCounter(m, BBName);
      BBCounterMap[StringRef(BBName)] = Var;

      Builder.SetInsertPoint(BB.getFirstNonPHI());
      // Load, inc, store
      LoadInst *Load = Builder.CreateLoad(Var, "ld");
      Value *Inc = Builder.CreateAdd(Builder.getInt32(1), Load);
      Builder.CreateStore(Inc, Var);
      ++numBBs;
      ++BBNum;
    }
  }

  // STEP 2: Write result to file
  PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(ctxt));
  FunctionType *PrintfTy = FunctionType::get(IntegerType::getInt32Ty(ctxt),
                                             PrintfArgTy, /*IsVarArgs=*/true);
  Function *Printf =
      dyn_cast<Function>(m.getOrInsertFunction("printf", PrintfTy));

  // STEP 3: Inject global variable that will hold printf's format string
  Constant *ResultFormatStr =
      ConstantDataArray::getString(ctxt, "%-20s %-10lu\n");
  Constant *ResultFormatStrVar =
      m.getOrInsertGlobal("ResultFormatStrIR", ResultFormatStr->getType());

  dyn_cast<GlobalVariable>(ResultFormatStrVar)->setInitializer(ResultFormatStr);
  std::string out = "";
  out += "=================================================\n";
  out += "NAME                 #N CALLS\n";
  out += "-------------------------------------------------\n";

  Constant *ResultHeaderStr = ConstantDataArray::getString(ctxt, out.c_str());
  Constant *ResultHeaderStrVar =
      m.getOrInsertGlobal("ResultHeaderStrIR", ResultHeaderStr->getType());
  dyn_cast<GlobalVariable>(ResultHeaderStrVar)->setInitializer(ResultHeaderStr);

  // STEP 4: Printf wrapper that will print the result
  FunctionType *PrintfWrapperTy =
      FunctionType::get(Type::getVoidTy(ctxt), {}, false);
  Function *PrintfWrapperF = dyn_cast<Function>(
      m.getOrInsertFunction("printf_wrapper", PrintfWrapperTy));

  if (PrintfWrapperF == nullptr)
    std::cerr << "PRintfWrapperF is NULL !";
  // Basic block for calling actual printf
  BasicBlock *RetBlock = BasicBlock::Create(ctxt, "enter", PrintfWrapperF);
  Builder.SetInsertPoint(RetBlock);
  Value *ResultHeaderStrPtr =
      Builder.CreatePointerCast(ResultHeaderStrVar, PrintfArgTy);
  Value *ResultFormatStrPtr =
      Builder.CreatePointerCast(ResultFormatStrVar, PrintfArgTy);

  Builder.CreateCall(Printf, {ResultHeaderStrPtr});

  for (auto &item : BBCounterMap) {
    LoadInst *LoadCounter =
        Builder.CreateLoad(item.getValue(), "ld_counter_print");
    Builder.CreateCall(
        Printf, ArrayRef<Value *>({ResultFormatStrPtr,
                                   Builder.CreateGlobalStringPtr(item.getKey()),
                                   LoadCounter}));
  }

  appendToGlobalDtors(m, PrintfWrapperF, /*Priority=*/0);

  // STEP 4.1: Write output to a file
  FunctionType *FopenType = FunctionType::get(
      Type::getInt8PtrTy(ctxt),
      ArrayRef<Type *>({Type::getInt8PtrTy(ctxt), Type::getInt8PtrTy(ctxt)}),
      false);
  FunctionType *FprintfType =
      FunctionType::get(Type::getInt32Ty(ctxt), Type::getInt8PtrTy(ctxt), true);
  FunctionType *FcloseType = FunctionType::get(Type::getInt32Ty(ctxt),
                                               Type::getInt8PtrTy(ctxt), false);

  Function *Fopen =
      dyn_cast<Function>(m.getOrInsertFunction("fopen", FopenType));
  Function *Ffprintf =
      dyn_cast<Function>(m.getOrInsertFunction("fprintf", FprintfType));
  Function *Fclose =
      dyn_cast<Function>(m.getOrInsertFunction("fclose", FcloseType));

  Value *Filename = Builder.CreateGlobalStringPtr(StringRef(outputFileName),
                                                  "outfilename_str");
  Value *Mode = Builder.CreateGlobalStringPtr(StringRef("w"), "mode_str");
  Value *FileHandle =
      Builder.CreateCall(Fopen, ArrayRef<Value *>({Filename, Mode}));

  Value *FormatHeaderStr = Builder.CreateGlobalStringPtr(
      StringRef("Total Blocks:%d\n"), "format_str");
  Value *NumBBsVal = ConstantInt::get(Type::getInt32Ty(ctxt), numBBs);
  Value *Args[] = {FileHandle, FormatHeaderStr, NumBBsVal};

  Builder.CreateCall(Ffprintf, Args);
  Value *FormatStr =
      Builder.CreateGlobalStringPtr(StringRef("%s:%d\n"), "format_str");

  for (auto &item : BBCounterMap) {
    LoadInst *LoadCounter =
        Builder.CreateLoad(item.getValue(), "ld_counter_print");

    Builder.CreateCall(
        Ffprintf,
        ArrayRef<Value *>({FileHandle, FormatStr,
                           Builder.CreateGlobalStringPtr(item.getKey()),
                           LoadCounter}));
  }

  Builder.CreateCall(Fclose, FileHandle);
  Builder.CreateRetVoid();
  appendToGlobalDtors(m, PrintfWrapperF, /*Priority=*/0);
  return true;
}
