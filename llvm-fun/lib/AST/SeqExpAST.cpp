#include "AST/SeqExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

SeqExpAST::SeqExpAST(SrcLoc srcLoc, ExpAST *exp1, ExpAST *exp2)
  : ExpAST(OP_Seq, srcLoc), exp1(exp1), exp2(exp2) {
  exp1->setParent(this);
  exp2->setParent(this);
}

SeqExpAST::~SeqExpAST() {
  delete exp1;
  delete exp2;
}

void SeqExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *SeqExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *SeqExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *SeqExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
