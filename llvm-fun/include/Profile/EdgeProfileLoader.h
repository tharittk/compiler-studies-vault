// It is assumed that there are no critical edges left in the program,
// e.g., you should run -break-crit-edges before this.

#ifndef FUN_PROFILE_EDGEPROFILELOADER_H
#define FUN_PROFILE_EDGEPROFILELOADER_H

#include "llvm/Pass.h"

namespace fun {

using namespace llvm;

class EdgeProfileLoader : public ModulePass {
public:
  static char ID; // Pass identification, replacement for typeid

  typedef std::pair<const BasicBlock*, const BasicBlock*> Edge;

  // Creates an Edge between two BasicBlocks.
  static Edge getEdge(const BasicBlock *src, const BasicBlock *dest) {
    return Edge(src, dest);
  }

  EdgeProfileLoader() : ModulePass(ID) {}
  bool runOnModule(Module &m);

  virtual const char *getPassName() const override {
    return "Edge Profile Loader";
  }

  void getAnalysisUsage(AnalysisUsage &au) const override {
    au.setPreservesAll();
  }

  // TODO Impelement these functions

  // This should return the execution count for the given edge
  unsigned getCount(const Edge &edge) const;
  // This should return the weight for the given edge
  double getWeight(const Edge &edge) const;

private:
  // TODO Add any member functions or variables here if you want to

};

}

#endif
