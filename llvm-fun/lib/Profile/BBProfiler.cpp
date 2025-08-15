#define DEBUG_TYPE "cfg-profiler"

#include "BBProfiler.h"
#include "ProfileUtils.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include <iostream>

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numBBs, "The # of BBs profiled.");

char BBProfiler::ID = 0;
static RegisterPass<BBProfiler> rp("bb-profiler", "Basic Block Profiler");

static cl::opt<std::string>
outputFileName("bb-info-output-file", cl::init("bb_info.prof"),
    cl::NotHidden, cl::Optional, cl::ValueRequired,
    cl::value_desc("filename"),
    cl::desc("Output profile info file (used by -bb-profiler)"));

bool BBProfiler::runOnModule(Module &m) {

  std::cout << "Run BB Profile ! \n";
  // First pass to collect number of BB
  for (auto &F : m){
    // std::cerr << std::string(F.getName())+ "\n";
    for (auto &BB : F){
      ++numBBs;
      // std::cerr << "--> " << std::string(BB.getName()) << "\n";
    }
  }

  ArrayType* arrayTy = ArrayType::get(Type::getInt32Ty(m.getContext()), 
                      (unsigned long long) numBBs);

  Constant* initializer = ConstantAggregateZero::get(arrayTy);
  // TODO (tharitt): Initialize to 0
  auto bbCounter =new GlobalVariable (arrayTy,
  /*isConstant*/false, GlobalValue::ExternalLinkage, initializer, "bbCounter");

  size_t global_index = 0;
  // Second pass: inject the counter increment instruction
  for (auto &F : m){
    for (auto &BB : F) {
      // Add 1 to the array indexed `global_index`
      BasicBlock::iterator insertionPoint = BB.begin();

      // Load bbCounter[global_index] to register
      Value* counterPtr = GetElementPtrInst::Create(bbCounter, 
        {ConstantInt::get(Type::getInt32Ty(m.getContext()), global_index)},
        "get.counterPtr", &*insertionPoint); 
      LoadInst* current_counter = new LoadInst(counterPtr, "current.counter", &*insertionPoint);
      // Binary Operation of r <- bbCounter[global_index] + 1
      ConstantInt *one = ConstantInt::get(Type::getInt32Ty(m.getContext()), 1);
      BinaryOperator* incr_val = BinaryOperator::Create(Instruction::Add, current_counter, one, "incr", &*insertionPoint);
      // Store result to r to bbCounter[global_index]
      StoreInst* storeInst = new StoreInst(incr_val, counterPtr, &*insertionPoint);
    }
  }
  return true;
}
