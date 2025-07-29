#include "AST/IdExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

void IdExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *IdExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *IdExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *IdExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
