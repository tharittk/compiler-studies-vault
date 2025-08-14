// It is assumed that there are no critical edges left in the program,
// e.g., you should run -break-crit-edges before this.

#ifndef FUN_PROFILE_PROFILEPRINTER_H
#define FUN_PROFILE_PROFILEPRINTER_H

#include "Profile/BBProfileLoader.h"
#include "Profile/EdgeProfileLoader.h"
#include "llvm/Pass.h"

namespace fun {

using namespace llvm;

class ProfilePrinter : public ModulePass {
public:
  static char ID; // Pass identification, replacement for typeid

  ProfilePrinter() : ModulePass(ID) {}
  bool runOnModule(Module &m);

  virtual const char *getPassName() const override {
    return "Profile Information Printer";
  }

  void getAnalysisUsage(AnalysisUsage &au) const override;

private:
  void dumpBBInfo(const Module &m, std::ofstream &fout);
  void dumpEdgeInfo(const Module &m, std::ofstream &fout);
};

}

#endif
