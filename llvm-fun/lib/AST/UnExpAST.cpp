#include "AST/UnExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

UnExpAST::UnExpAST(OpKind op, SrcLoc srcLoc, ExpAST *exp1)
  : ExpAST(op, srcLoc), exp1(exp1) {
  exp1->setParent(this);
}

UnExpAST::~UnExpAST() { delete exp1; }

// For LLVM-styld RTTI support
bool UnExpAST::classof(const BaseAST *n) {
  switch (n->getOp()) {
  case OP_Ref:
  case OP_Get:
  case OP_UMinus:
  case OP_Not:
    return true;
  default:
    return false;
  }
}

void UnExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *UnExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *UnExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *UnExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
