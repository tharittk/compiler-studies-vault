#define DEBUG_TYPE "cfg-profiler"

#include "ProfileUtils.h"

namespace fun {
using namespace llvm;
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

void InsertWriteResultIR(Module &m, LLVMContext &ctxt,
                         std::string outputFileName, Statistic Count,
                         StringMap<Value *> &CounterMap) {
  IRBuilder<> Builder(ctxt);

  FunctionType *PrintfWrapperTy =
      FunctionType::get(Type::getVoidTy(ctxt), {}, false);
  Function *PrintfWrapperF = dyn_cast<Function>(m.getOrInsertFunction(
      "printf_wrapper_" + outputFileName, PrintfWrapperTy));

  /* Call this at the end of module */
  appendToGlobalDtors(m, PrintfWrapperF, /*Priority=*/0);
  BasicBlock *RetBlock = BasicBlock::Create(ctxt, "enter", PrintfWrapperF);
  Builder.SetInsertPoint(RetBlock);

  // Write result to file
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

  // Total count
  Value *FormatHeaderStr =
      Builder.CreateGlobalStringPtr(StringRef("ALL:%d\n"), "format_str");
  Value *CountVal = ConstantInt::get(Type::getInt32Ty(ctxt), Count);
  Builder.CreateCall(
      Ffprintf, ArrayRef<Value *>({FileHandle, FormatHeaderStr, CountVal}));

  Value *FormatStr =
      Builder.CreateGlobalStringPtr(StringRef("%s:%d\n"), "format_str");

  for (auto &item : CounterMap) {
    LoadInst *LoadCounter =
        Builder.CreateLoad(item.getValue(), "ld_counter_printE");

    Builder.CreateCall(
        Ffprintf,
        ArrayRef<Value *>({FileHandle, FormatStr,
                           Builder.CreateGlobalStringPtr(item.getKey()),
                           LoadCounter}));
  }
  Builder.CreateCall(Fclose, FileHandle);
  Builder.CreateRetVoid();
}

} // namespace fun