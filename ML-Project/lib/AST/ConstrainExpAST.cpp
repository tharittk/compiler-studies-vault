#include "AST/ConstrainExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

ConstrainExpAST::ConstrainExpAST(SrcLoc srcLoc, ExpAST *exp, TypeAST *type)
  : ExpAST(OP_Constrain, srcLoc), exp(exp), type(type) {
  exp->setParent(this);
  type->setParent(this);
}

ConstrainExpAST::~ConstrainExpAST() {
  delete exp;
  delete type;
}

void ConstrainExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *ConstrainExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *ConstrainExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *ConstrainExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
