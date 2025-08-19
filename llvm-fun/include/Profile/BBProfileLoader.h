// It is assumed that there are no critical edges left in the program,
// e.g., you should run -break-crit-edges before this.

#ifndef FUN_PROFILE_BBPROFILELOADER_H
#define FUN_PROFILE_BBPROFILELOADER_H

#include "llvm/Pass.h"

namespace fun {

using namespace llvm;

class BBProfileLoader : public ModulePass {
public:
  static char ID; // Pass identification, replacement for typeid

  DenseMap<uint64_t, std::string> BBToBBName;
  StringMap<unsigned> BBNameToCount;

  BBProfileLoader() : ModulePass(ID) {}
  bool runOnModule(Module &m);

  virtual const char *getPassName() const override {
    return "Basic Block Profile Loader";
  }

  void getAnalysisUsage(AnalysisUsage &au) const override {
    au.setPreservesAll();
  }

  // TODO Impelement this function

  // This should return the execution count for the given basic block
  unsigned getCount(const BasicBlock *bb) const;

private:
  // TODO Add any member functions or variables here if you want to
};

} // namespace fun

#endif
