#ifndef FUN_SUPERBLOCK_SUPERBLOCKFORMATION_H
#define FUN_SUPERBLOCK_SUPERBLOCKFORMATION_H

#include "Profile/BBProfileLoader.h"
#include "Profile/EdgeProfileLoader.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
namespace fun {

using namespace llvm;

class SuperblockFormation : public ModulePass {
public:
  static char ID; // Pass identification, replacement for typeid

  SuperblockFormation() : ModulePass(ID) {}

  bool runOnModule(Module &m);

  virtual const char *getPassName() const override {
    return "Superblock formation";
  }

  void getAnalysisUsage(AnalysisUsage &au) const override {
    au.addRequired<LoopInfo>();
    au.addRequired<BBProfileLoader>();
    au.addRequired<EdgeProfileLoader>();
  }

private:
  BasicBlock *selectTrace(Module &m);
  BasicBlock *getBestPredecessorOf(BasicBlock *BB);
  BasicBlock *getBestPredecessorOf(BasicBlock *BB);
};

} // namespace fun

#endif
