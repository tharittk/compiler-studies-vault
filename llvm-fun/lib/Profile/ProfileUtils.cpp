#define DEBUG_TYPE "cfg-profiler"

#include "ProfileUtils.h"

namespace fun {
using namespace llvm;
GlobalVariable *CreateGlobalCounter(Module &m, StringRef VarName) {
  auto &ctxt = m.getContext();
  // declaration
  Constant *NewGlobalVar =
      m.getOrInsertGlobal(VarName, IntegerType::getInt32Ty(ctxt));
  // definition
  GlobalVariable *NewGV = m.getNamedGlobal(VarName);
  NewGV->setLinkage(GlobalValue::CommonLinkage);
  // NewGV->setAlignment(4);
  NewGV->setInitializer(ConstantInt::get(ctxt, APInt(32, 0)));
  return NewGV;
}
} // namespace fun