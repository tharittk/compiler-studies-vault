#include "AST/TupleExpAST.h"
#include "AST/ExpASTList.h"
#include "Visitor/Visitors.h"

using namespace fun;

TupleExpAST::TupleExpAST(SrcLoc srcLoc, ExpASTList *list)
  : ExpAST(OP_Tuple, srcLoc), exps(list->getExpList()) {
  for (auto &e : exps)
    e->setParent(this);
}

TupleExpAST::TupleExpAST(SrcLoc srcLoc, std::vector<ExpAST*> &exps)
  : ExpAST(OP_Tuple, srcLoc), exps(exps) {
  for (auto &e : exps)
    e->setParent(this);
}

TupleExpAST::~TupleExpAST() {
  for (auto &e : exps)
    delete e;
}

void TupleExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *TupleExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *TupleExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *TupleExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
