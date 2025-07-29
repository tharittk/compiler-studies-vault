#include "AST/CallExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

CallExpAST::CallExpAST(SrcLoc srcLoc, ExpAST *funExp, ExpAST *argExp)
  : ExpAST(OP_Call, srcLoc), funExp(funExp), argExp(argExp) {
  funExp->setParent(this);
  argExp->setParent(this);
}

CallExpAST::~CallExpAST() {
  delete funExp;
  delete argExp;
}

void CallExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *CallExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *CallExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *CallExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
