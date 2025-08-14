// It is assumed that there are no critical edges left in the program,
// e.g., you should run -break-crit-edges before this.

#ifndef FUN_PROFILE_EDGEPROFILER_H
#define FUN_PROFILE_EDGEPROFILER_H

#include "llvm/Pass.h"

namespace fun {

using namespace llvm;

class EdgeProfiler : public ModulePass {
public:
  static char ID; // Pass identification, replacement for typeid

  EdgeProfiler() : ModulePass(ID) {}

  bool runOnModule(Module &m);

  virtual const char *getPassName() const override {
    return "Edge Profiler";
  }

  void getAnalysisUsage(AnalysisUsage &au) const override {
    au.setPreservesCFG();
  }
};

}

#endif
