#include "AST/TupleTypeAST.h"
#include "AST/TypeASTList.h"
#include "Visitor/Visitors.h"

using namespace fun;

TupleTypeAST::TupleTypeAST(SrcLoc srcLoc, TypeASTList *list)
  : TypeAST(OP_TupleType, srcLoc), types(list->getTypeASTList()) {
  for (auto &t : types)
    t->setParent(this);
}

TupleTypeAST::TupleTypeAST(SrcLoc srcLoc, std::vector<TypeAST*> &types)
  : TypeAST(OP_TupleType, srcLoc), types(types) {
  for (auto &t : types)
    t->setParent(this);
}

TupleTypeAST::~TupleTypeAST() {
  for (auto &t : types)
    delete t;
}

MyTupleType *TupleTypeAST::getType() const {
  std::vector<MyType*> matchingTypes;
  for (auto &t : types)
    matchingTypes.push_back(MyType::getType(t));
  return MyType::getTupleType(matchingTypes);
}

void TupleTypeAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *TupleTypeAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *TupleTypeAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *TupleTypeAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
