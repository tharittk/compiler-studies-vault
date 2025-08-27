#define DEBUG_TYPE "cfg-profiler"

#include "EdgeProfiler.h"
#include "ProfileUtils.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include <iostream>

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numEdges, "The # of edges profiled.");

char EdgeProfiler::ID = 0;
static RegisterPass<EdgeProfiler> rp("edge-profiler", "Edge Profiler");

static cl::opt<std::string> outputFileName(
    "edge-info-output-file", cl::init("edge_info.prof"), cl::NotHidden,
    cl::Optional, cl::ValueRequired, cl::value_desc("filename"),
    cl::desc("Output profile info file (used by -edge-profiler)"));

bool EdgeProfiler::runOnModule(Module &m) {
  auto &ctxt = m.getContext();

  IRBuilder<> Builder(ctxt);
  for (auto &F : m) {
    unsigned BBNum = 0;
    for (auto &BB : F) {
      TerminatorInst *Terminator = BB.getTerminator();

      for (unsigned i = 0, e = Terminator->getNumSuccessors(); i != e; ++i) {

        BasicBlock *NewBB = SplitCriticalEdge(&BB, Terminator->getSuccessor(i));

        if (NewBB) {
          // If the edge was split, the new block is the perfect place
          // for the counter.
          Builder.SetInsertPoint(NewBB->getTerminator());
        } else {
          // If the edge was not critical, place the counter at the
          // beginning of the successor block.
          BasicBlock *Succ = Terminator->getSuccessor(i);
          Builder.SetInsertPoint(Succ->getFirstNonPHI());
        }

        std::string EdgeName =
            BB.getName().str() + Terminator->getSuccessor(i)->getName().str();
        GlobalVariable *Var = CreateGlobalCounter(m, EdgeName);
        EdgeCounterMap[StringRef(EdgeName)] = Var;

        LoadInst *Load = Builder.CreateLoad(Var, "ld.edge.count");
        Value *Inc = Builder.CreateAdd(Builder.getInt32(1), Load);

        Builder.CreateStore(Inc, Var);
        ++numEdges;
      }
      ++BBNum;
    }
  }

  InsertWriteResultIR(m, ctxt, outputFileName, numEdges, EdgeCounterMap);

  return true;
}
