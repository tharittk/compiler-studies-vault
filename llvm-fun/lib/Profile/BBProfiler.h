// It is assumed that there are no critical edges left in the program,
// e.g., you should run -break-crit-edges before this.

#ifndef FUN_PROFILE_BBPROFILER_H
#define FUN_PROFILE_BBPROFILER_H

#include "llvm/Pass.h"

namespace fun {

using namespace llvm;

class BBProfiler : public ModulePass {
public:
  static char ID; // Pass identification, replacement for typeid

  BBProfiler() : ModulePass(ID) {}

  bool runOnModule(Module &m);

  virtual const char *getPassName() const override {
    return "Basic Block Profiler";
  }

  void getAnalysisUsage(AnalysisUsage &au) const override {
    au.setPreservesCFG();
  }
};

}

#endif
