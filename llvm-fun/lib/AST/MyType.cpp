#include "AST/MyType.h"
#include "AST/TypeAST.h"
#include "llvm/Support/Casting.h"
#include <assert.h>
#include <sstream>

using namespace fun;

MyTypeFactory &MyType::factory = MyTypeFactory::getFactory();

MyIntType *MyType::getIntType() {
  return factory.getIntType();
}

MyRefType *MyType::getRefType(MyType *baseType) {
  return factory.getRefType(baseType);
}

MyFunType *MyType::getFunType(MyType *paramType, MyType *retType) {
  return factory.getFunType(paramType, retType);
}

MyTupleType *MyType::getTupleType(const std::vector<MyType*> &types) {
  return factory.getTupleType(types);
}

MyTupleType *MyType::getUnitType() {
  return factory.getUnitType();
}

MyIntType *MyType::toIntType(MyType *t) {
  return llvm::dyn_cast<MyIntType>(t);
}

MyRefType *MyType::toRefType(MyType *t) {
  return llvm::dyn_cast<MyRefType>(t);
}

MyFunType *MyType::toFunType(MyType *t) {
  return llvm::dyn_cast<MyFunType>(t);
}

MyTupleType *MyType::toTupleType(MyType *t) {
  return llvm::dyn_cast<MyTupleType>(t);
}

std::string MyTupleType::toString() const {
  std::stringstream ss;
  ss << "<";
  std::vector<MyType*>::const_iterator it;
  for (it = types.begin(); it != types.end(); ++it) {
    ss << (*it)->toString();
    if (it + 1 != types.end()) ss << ", ";
  }
  ss << ">";
  return ss.str();
}

MyType *MyType::getType(TypeAST *tn) {
  return tn->getType();
}

MyTypeFactory *MyTypeFactory::factoryInstance = nullptr;

MyTypeFactory::~MyTypeFactory() {
  for (auto &t : refTypes)
    delete t;
  for (auto &t : funTypes)
    delete t;
  for (auto &t : tupleTypes)
    delete t;
}

MyRefType *MyTypeFactory::getRefType(MyType *baseType) {
  for (auto &t : refTypes) {
    if (t->getBaseType() == baseType)
      return t;
  }
  MyRefType *t = new MyRefType(baseType);
  refTypes.push_back(t);
  return t;
}

MyFunType *MyTypeFactory::getFunType(MyType *paramType, MyType *retType) {
  for (auto &t : funTypes) {
    if (t->getParamType() == paramType && t->getRetType() == retType)
      return t;
  }
  MyFunType *t = new MyFunType(paramType, retType);
  funTypes.push_back(t);
  return t;
}

MyTupleType *MyTypeFactory::getTupleType(const std::vector<MyType*> &types) {
  for (auto &t : tupleTypes) {
    if (types == t->getTypes())
      return t;
  }
  MyTupleType *t = new MyTupleType(types);
  tupleTypes.push_back(t);
  return t;
}
