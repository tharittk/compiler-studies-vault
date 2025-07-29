#include "AST/IfExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

IfExpAST::IfExpAST(SrcLoc srcLoc, ExpAST *condExp, ExpAST *thenExp,
                   ExpAST *elseExp)
  : ExpAST(elseExp ? OP_IfThenElse : OP_IfThen, srcLoc), condExp(condExp),
    thenExp(thenExp), elseExp(elseExp) {
  condExp->setParent(this);
  thenExp->setParent(this);
  if (elseExp) elseExp->setParent(this);
}

IfExpAST::~IfExpAST() {
  delete condExp;
  delete thenExp;
  delete elseExp;
}

void IfExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *IfExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *IfExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *IfExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
