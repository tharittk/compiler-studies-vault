#define DEBUG_TYPE "cfg-profiler"
#include "llvm/IR/Value.h"
#include "llvm/IR/BasicBlock.h"
#include "BBProfiler.h"
#include "ProfileUtils.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
// #include "llvm/Support/Alignment.h"
#include <iostream>

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numBBs, "The # of BBs profiled.");

char BBProfiler::ID = 0;
static RegisterPass<BBProfiler> rp("bb-profiler", "Basic Block Profiler");

static cl::opt<std::string>
outputFileName("bb-info-output-file", cl::init("bb_info.prof"),
    cl::NotHidden, cl::Optional, cl::ValueRequired,
    cl::value_desc("filename"),
    cl::desc("Output profile info file (used by -bb-profiler)"));

GlobalVariable* CreateGlobalCounter(Module &m, StringRef VarName){
  auto& ctxt = m.getContext();
  // declaration
  Constant *NewGlobalVar = m.getOrInsertGlobal(VarName, IntegerType::getInt32Ty(ctxt));
  // definition
  GlobalVariable *NewGV = m.getNamedGlobal(VarName);
  NewGV->setLinkage(GlobalValue::CommonLinkage);
  // NewGV->setAlignment(4);
  NewGV->setInitializer(ConstantInt::get(ctxt, APInt(32, 0)));
  return NewGV;
  // return new GlobalVariable(m, Type::getInt32Ty(ctxt),/*isConstant*/false,
    // GlobalValue::ExternalLinkage, ConstantInt::get(Type::getInt32Ty(ctxt), 0));
}

bool BBProfiler::runOnModule(Module &m) {
  auto& ctxt = m.getContext(); 

  std::cout << "Run BB Profile ! \n";

  IRBuilder<> Builder(ctxt);
  for (auto &F : m){
    for (auto &BB : F){
      // Create global counter for each basic block
      // This must be a constant address because we basically
      // encode that address info for code generation
      std::string CounterName = "CounterID_" + std::to_string(numBBs);
      GlobalVariable* Var = CreateGlobalCounter(m, CounterName);
      BBCounterMap.insert(std::make_pair(&BB, Var));

      Builder.SetInsertPoint(BB.getFirstNonPHI());
      // Load, inc, store
      LoadInst *Load = Builder.CreateLoad(Var, "ld");
      Value* Inc = Builder.CreateAdd(Builder.getInt32(1), Load);
      Builder.CreateStore(Inc, Var);
      ++numBBs;
    }
  }
  return true;
}
