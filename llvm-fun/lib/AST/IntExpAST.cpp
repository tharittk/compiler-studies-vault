#include "AST/IntExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

void IntExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *IntExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *IntExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *IntExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
