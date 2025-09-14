#include "AST/IntTypeAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

MyIntType *IntTypeAST::getType() const {
  return MyType::getIntType();
}

void IntTypeAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *IntTypeAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *IntTypeAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *IntTypeAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
