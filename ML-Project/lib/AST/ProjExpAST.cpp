#include "AST/ProjExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

ProjExpAST::ProjExpAST(SrcLoc srcLoc, int index, ExpAST *targetTupleExp)
  : ExpAST(OP_Proj, srcLoc), index(index), targetTupleExp(targetTupleExp) {
  targetTupleExp->setParent(this);
}

ProjExpAST::~ProjExpAST() {
  delete targetTupleExp;
}

void ProjExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *ProjExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *ProjExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *ProjExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
