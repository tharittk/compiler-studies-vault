#include "AST/BinExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

BinExpAST::BinExpAST(OpKind op, SrcLoc srcLoc, ExpAST *exp1, ExpAST *exp2)
  : ExpAST(op, srcLoc), exp1(exp1), exp2(exp2) {
  exp1->setParent(this);
  exp2->setParent(this);
}

BinExpAST::~BinExpAST() {
  delete exp1;
  delete exp2;
}

// For LLVM-styld RTTI support
bool BinExpAST::classof(const BaseAST *n) {
  switch (n->getOp()) {
  case OP_Mul:
  case OP_Add:
  case OP_Sub:
  case OP_Equal:
  case OP_LT:
  case OP_And:
  case OP_Or:
  case OP_Set:
    return true;
  default:
    return false;
  }
}

void BinExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *BinExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *BinExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *BinExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
