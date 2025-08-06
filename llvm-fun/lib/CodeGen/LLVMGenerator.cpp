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

Value *LLVMGenerator::visit(BinExpAST *n) {
  auto leftExp = n->getExp1AST();
  auto rightExp = n->getExp2AST();

  Value *L = leftExp->accept(*this);
  Value *R = rightExp->accept(*this);
  if (!L || !R)
    reportErrorAndExit(n->getSrcLoc(), "BinExp Error: L or R Value is null");

  switch (n->getOp()) {
    case OpKind::OP_Add:
      return this->builder.CreateAdd(L, R, "addf");
    case OpKind::OP_Sub:
      return this->builder.CreateSub(L, R, "subf");
    case OpKind::OP_Mul:
      return this->builder.CreateMul(L, R, "mulf");
    case OpKind::OP_Equal:
      return this->builder.CreateICmp(CmpInst::ICMP_EQ, L, R, "eqf");
    case OpKind::OP_LT:
      return this->builder.CreateICmp(CmpInst::ICMP_SLT, L, R, "ltf");
    case OpKind::OP_And:
      return this->builder.CreateAnd(L, R, "andf");
    case OpKind::OP_Or:
      return this->builder.CreateOr(L, R, "orf");
    case OpKind::OP_Set:{
      // auto Int32Ty = Type::getInt32Ty(getGlobalContext());
      // AllocaInst *Ptr = this->builder.CreateAlloca(Int32Ty, L, "ptr");
      return this->builder.CreateStore(R, L);
    }
    default:{
      reportErrorAndExit(n->getSrcLoc(), "BinExp Error: No case matches");
      return nullptr;
    }
  }
}

Value *LLVMGenerator::visit(CallExpAST *n) {
  auto funExp = n->getFunExpAST();
  auto argExp = n->getArgExpAST();

  // simple because our language supports only one variable
  Value* funV = funExp->accept(*this);
  Value* argV = argExp->accept(*this);

  if (!dyn_cast<Function> (funV))
    reportErrorAndExit(n->getSrcLoc(), "CallExp Error: dyn_cast test of funV must be Function Type");

  return builder.CreateCall(funV, argV, "callf");
}

Value *LLVMGenerator::visit(ConstrainExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(FunDeclAST *n) {



}

Value *LLVMGenerator::visit(IdExpAST *n) {

  if (!ctxt.has(n->getName())) {
    auto msg = "IdExp Error: "+ n->getName() + " is not in env";
    reportErrorAndExit(n->getSrcLoc(), msg);
  }
  return ctxt.get(n->getName());
}

Value *LLVMGenerator::visit(IfExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(IntExpAST *n) {
  return ConstantInt::get(llvm::Type::getInt32Ty(getGlobalContext()), n->getNum(), /*isSigned=*/true);
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
