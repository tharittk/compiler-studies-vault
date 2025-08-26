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

bool BBProfiler::runOnModule(Module &m) {
  auto &ctxt = m.getContext();

  IRBuilder<> Builder(ctxt);
  // STEP 1: Code Injection
  for (auto &F : m) {
    unsigned BBNum = 0; // BB numbering
    for (auto &BB : F) {
      // Create global counter for each basic block
      // This must be a constant address because we basically
      // encode that address info for code generation
      // TODO: Use nmae from instnamer pass !!! (required then)
      std::string BBName =
          F.getName().str() + std::string("_BB_") + std::to_string(BBNum);

      GlobalVariable *Var = CreateGlobalCounter(m, BBName);
      BBCounterMap[StringRef(BBName)] = Var;

      Builder.SetInsertPoint(BB.getFirstNonPHI());
      // Load, inc, store
      LoadInst *Load = Builder.CreateLoad(Var, "ld.block.count");
      Value *Inc = Builder.CreateAdd(Builder.getInt32(1), Load);
      Builder.CreateStore(Inc, Var);
      ++numBBs;
      ++BBNum;
    }
  }

  InsertWriteResultIR(m, ctxt, outputFileName, numBBs, BBCounterMap);

  return true;
}
