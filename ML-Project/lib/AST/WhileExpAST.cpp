#include "AST/WhileExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

WhileExpAST::WhileExpAST(SrcLoc srcLoc, ExpAST *condExp, ExpAST *bodyExp)
  : ExpAST(OP_While, srcLoc), condExp(condExp), bodyExp(bodyExp) {
  condExp->setParent(this);
  bodyExp->setParent(this);
}

WhileExpAST::~WhileExpAST() {
  delete condExp;
  delete bodyExp;
}

void WhileExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *WhileExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *WhileExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *WhileExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
