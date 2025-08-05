#include "CodeGen/LLVMGenerator.h"
#include "AST/AST.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

using namespace fun;
using namespace llvm;

void LLVMGenerator::run(std::ostream &out) {
  InitializeNativeTarget();
  module = new Module(srcFileName, getGlobalContext());

  // Actually we don't need JIT feature in this assignment, but we create this
  // to get DataLayout and TargetMachine instances.
  std::string errStr;
  ExecutionEngine *engine = EngineBuilder(module).setErrorStr(&errStr).create();
  if (!engine) {
    std::cerr << "Could not create ExecutionEngine: " << errStr << "\n";
    exit(1);
  }
  module->setDataLayout(engine->getDataLayout());
  module->setTargetTriple(engine->getTargetMachine()->getTargetTriple());

  visit(prg);

  // Print out all of the generated code.
  std::string buf;
  raw_string_ostream os(buf);
  os << *module;
  os.flush();
  out << buf;
  delete engine;
}

Value *LLVMGenerator::allocHeap(Type *t, BasicBlock *bb, std::string name) {
  const DataLayout *layout = module->getDataLayout();
  ConstantInt *allocSize =
    ConstantInt::get(Type::getInt32Ty(getGlobalContext()),
                     layout->getTypeAllocSize(t));

  Instruction *inst = CallInst::CreateMalloc(
      bb,
      Type::getInt32Ty(getGlobalContext()),
      t,
      allocSize,
      nullptr,
      nullptr,
      name);
  bb->getInstList().push_back(inst);
  return inst;
}

// TODO Implement all visit functions and other helper functions (if any) below

Value *LLVMGenerator::visit(BinExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(CallExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(ConstrainExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(FunDeclAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(IdExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(IfExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(IntExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(LetExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(ProgramAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(ProjExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(SeqExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(TupleExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(UnExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(WhileExpAST *n) {
  return NULL;
}
