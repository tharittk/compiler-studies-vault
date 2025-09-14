#ifndef FUN_PROFILE_PROFILEUTILS_H
#define FUN_PROFILE_PROFILEUTILS_H

// TODO If you want to add some helper functions required from multiple
// classes, do it here
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

#include "llvm/IR/IRBuilder.h"
namespace fun {
using namespace llvm;

GlobalVariable *CreateGlobalCounter(Module &m, StringRef VarName);

void InsertWriteResultIR(Module &m, LLVMContext &ctxx,
                         std::string outputFileName, Statistic Count,
                         StringMap<Value *> &CounterMap);
} // namespace fun
#endif