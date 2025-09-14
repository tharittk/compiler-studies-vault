#include "AST/RefTypeAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

RefTypeAST::RefTypeAST(SrcLoc srcLoc, TypeAST *baseType)
  : TypeAST(OP_RefType, srcLoc), baseType(baseType) {
  baseType->setParent(this);
}

RefTypeAST::~RefTypeAST() {
  delete baseType;
}

MyRefType *RefTypeAST::getType() const {
  return MyType::getRefType(MyType::getType(baseType));
}

void RefTypeAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *RefTypeAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *RefTypeAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *RefTypeAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
