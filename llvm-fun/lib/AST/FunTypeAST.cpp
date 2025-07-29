#include "AST/FunTypeAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

FunTypeAST::FunTypeAST(SrcLoc srcLoc, TypeAST *paramType, TypeAST *retType)
  : TypeAST(OP_FunType, srcLoc), paramType(paramType), retType(retType) {
  paramType->setParent(this);
  retType->setParent(this);
}

FunTypeAST::~FunTypeAST() {
  delete paramType;
  delete retType;
}

MyFunType *FunTypeAST::getType() const {
  return MyType::getFunType(MyType::getType(paramType),
                            MyType::getType(retType));
}

void FunTypeAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *FunTypeAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *FunTypeAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *FunTypeAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
