#define DEBUG_TYPE "cfg-profiler"

#include "EdgeProfiler.h"
#include "ProfileUtils.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/CommandLine.h"
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

      if (dyn_cast<ReturnInst>(Terminator))
        continue;

      unsigned NumSucc = Terminator->getNumSuccessors();

      if (NumSucc == 1) {
        // fall through or uncondition branch
        // insert increment instruction at the end of this block
        std::string EdgeName = F.getName().str() + std::string("_BB_") +
                               std::to_string(BBNum) + std::string("_E_") +
                               std::to_string(0);

        /* End of the basic block */
        Builder.SetInsertPoint(BB.getTerminator());

        GlobalVariable *Var = CreateGlobalCounter(m, EdgeName);
        EdgeCounterMap[StringRef(EdgeName)] = Var;

        LoadInst *Load = Builder.CreateLoad(Var, "ld.egdge.count");
        Value *Inc = Builder.CreateAdd(Builder.getInt32(1), Load);
        Builder.CreateStore(Inc, Var);
        ++numEdges;

        std::cerr << "---Add Edge: " << EdgeName << std::endl;
      } else if (NumSucc >= 2) {
        // if-else, conditional, or switch instrction
        // insert instruction at the top of successor block
        unsigned EdgeIndex = 0;
        for (auto succ = succ_begin(&BB); succ != succ_end(&BB); ++succ) {

          std::string EdgeName = F.getName().str() + std::string("_BB_") +
                                 std::to_string(BBNum) + std::string("_E_") +
                                 std::to_string(EdgeIndex);
          Builder.SetInsertPoint((*succ)->getFirstNonPHI());
          ++EdgeIndex;
          ++numEdges;
          std::cerr << "Add Edge: " << EdgeName << std::endl;
        }
      } else {
        //  Not handled in this scope
        std::cerr << "Case not handled" << std::endl;
      }

      ++BBNum;
    }
  }

  return true;
}
