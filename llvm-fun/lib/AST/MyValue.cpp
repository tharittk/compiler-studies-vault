#include "AST/MyValue.h"
#include "llvm/Support/Casting.h"
#include <assert.h>
#include <sstream>

using namespace fun;

MyValueFactory &MyValue::factory = MyValueFactory::getFactory();

MyIntValue *MyValue::getIntValue(int num) {
  return factory.getIntValue(num);
}

MyRefValue *MyValue::getRefValue(MyValue *baseValue) {
  return factory.getRefValue(baseValue);
}

MyFunValue *MyValue::getFunValue(std::string name) {
  return factory.getFunValue(name);
}

MyTupleValue *MyValue::getTupleValue(const std::vector<MyValue*> &values) {
  return factory.getTupleValue(values);
}

MyTupleValue *MyValue::getUnitValue() {
  return factory.getUnitValue();
}

MyIntValue *MyValue::toIntValue(MyValue *v) {
  return llvm::dyn_cast<MyIntValue>(v);
}

MyRefValue *MyValue::toRefValue(MyValue *v) {
  return llvm::dyn_cast<MyRefValue>(v);
}

MyFunValue *MyValue::toFunValue(MyValue *v) {
  return llvm::dyn_cast<MyFunValue>(v);
}

MyTupleValue *MyValue::toTupleValue(MyValue *v) {
  return llvm::dyn_cast<MyTupleValue>(v);
}

std::string MyTupleValue::toString() const {
  std::stringstream ss;
  ss << "<";
  std::vector<MyValue*>::const_iterator it;
  for (it = values.begin(); it != values.end(); ++it) {
    ss << (*it)->toString();
    if (it + 1 != values.end()) ss << ", ";
  }
  ss << ">";
  return ss.str();
}

MyValueFactory *MyValueFactory::factoryInstance = nullptr;

MyValueFactory::~MyValueFactory() {
  for (auto &v : intValues)
    delete v;
  for (auto &v : refValues)
    delete v;
  for (auto &v : funValues)
    delete v;
  for (auto &v : tupleValues)
    delete v;
}

MyIntValue *MyValueFactory::getIntValue(int num) {
  for (auto &v : intValues) {
    if (v->getNum() == num)
      return v;
  }
  MyIntValue *v = new MyIntValue(num);
  intValues.push_back(v);
  return v;
}

MyRefValue *MyValueFactory::getRefValue(MyValue *baseValue) {
  // For RefValue, we create a new storage and thus a new RefValue instance
  // every time we encounter 'ref exp'.
  MyRefValue *v = new MyRefValue(baseValue);
  refValues.push_back(v);
  return v;
}

MyFunValue *MyValueFactory::getFunValue(std::string name) {
  for (auto &v : funValues) {
    if (v->getName() == name)
      return v;
  }
  MyFunValue *v = new MyFunValue(name);
  funValues.push_back(v);
  return v;
}

MyTupleValue *MyValueFactory::getTupleValue(const std::vector<MyValue*> &values)
{
  for (auto &v : tupleValues) {
    if (values == v->getValues())
      return v;
  }
  MyTupleValue *v = new MyTupleValue(values);
  tupleValues.push_back(v);
  return v;
}
