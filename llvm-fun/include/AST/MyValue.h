#ifndef FUN_AST_MYVALUE_H
#define FUN_AST_MYVALUE_H

#include <vector>
#include <string>
#include <sstream>

namespace fun {

class MyIntValue;
class MyRefValue;
class MyFunValue;
class MyTupleValue;
class MyValueFactory;

// Base class for all values. This if used by interpreter.
class MyValue {
  friend class MyValueFactory;

public:
  // Use these methods to get instances of value subclasses; you are not allowed
  // to instantiate subclasses of MyValue class directly by calling 'new'.
  static MyIntValue *getIntValue(int num);
  static MyRefValue *getRefValue(MyValue *baseValue);
  static MyFunValue *getFunValue(std::string name);
  static MyTupleValue *getTupleValue(const std::vector<MyValue*> &values);
  static MyTupleValue *getUnitValue(); // empty tuple

  // Downcasts the given MyValue class pointer into one of its child class
  // pointers. Returns null if the casting cannot be done.
  static MyIntValue *toIntValue(MyValue *v);
  static MyRefValue *toRefValue(MyValue *v);
  static MyFunValue *toFunValue(MyValue *v);
  static MyTupleValue *toTupleValue(MyValue *v);

  // Returns true if two value instances are semantically equal
  static bool equals(const MyValue *v1, const MyValue *v2) { return v1 == v2; }

  // Returns string representation of this value
  virtual std::string toString() const = 0;

  // Value testing
  virtual bool isIntValue() const { return false; }
  virtual bool isRefValue() const { return false; }
  virtual bool isFunValue() const { return false; }
  virtual bool isTupleValue() const { return false; }

  // Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
  enum ValueKind {
    VK_Int,
    VK_Ref,
    VK_Fun,
    VK_Tuple
  };
  ValueKind getKind() const { return kind; }

protected:
  explicit MyValue(ValueKind k) : kind(k) {}
  virtual ~MyValue() {}

private:
  static MyValueFactory &factory;
  const ValueKind kind;
};

class MyIntValue : public MyValue {
  friend class MyValueFactory;

public:
  int getNum() const { return num; }
  virtual bool isIntValue() const { return true; }
  virtual std::string toString() const {
    std::stringstream ss;
    ss << num;
    return ss.str();
  }

  // For LLVM-style RTTI support
  static bool classof(const MyValue *v) { return v->getKind() == VK_Int; }

private:
  MyIntValue(int num) : MyValue(VK_Int), num(num) {}
  int num;
};

class MyRefValue : public MyValue {
  friend class MyValueFactory;

public:
  MyValue *getBaseValue() { return baseValue; }
  void setBaseValue(MyValue *v) { baseValue = v; }
  virtual bool isRefValue() const { return true; }

  virtual std::string toString() const {
    std::stringstream ss;
    if (baseValue->isFunValue()) ss << "(";
    ss << "ref ";
    ss << baseValue->toString();
    if (baseValue->isFunValue()) ss << ")";
    return ss.str();
  }

  // For LLVM-style RTTI support
  static bool classof(const MyValue *v) { return v->getKind() == VK_Ref; }

private:
  explicit MyRefValue(MyValue *baseValue)
    : MyValue(VK_Ref), baseValue(baseValue) {}
  MyValue *baseValue;
};

class MyFunValue : public MyValue {
  friend class MyValueFactory;

public:
  std::string getName() { return name; }
  virtual bool isFunValue() const { return true; }

  virtual std::string toString() const { return name; }

  // For LLVM-style RTTI support
  static bool classof(const MyValue *v) { return v->getKind() == VK_Fun; }

private:
  MyFunValue(std::string name) : MyValue(VK_Fun), name(name) {}
  std::string name;
};

class MyTupleValue : public MyValue {
  friend class MyValueFactory;

public:
  int getLength() const { return values.size(); }
  MyValue *getValue(int idx) { return values[idx]; }
  const std::vector<MyValue*> &getValues() const { return values; }
  virtual bool isTupleValue() const { return true; }

  virtual std::string toString() const;

  // For LLVM-style RTTI support
  static bool classof(const MyValue *v) { return v->getKind() == VK_Tuple; }

private:
  explicit MyTupleValue(const std::vector<MyValue*> &values)
    : MyValue(VK_Tuple), values(values) {}
  std::vector<MyValue*> values;
};

class MyValueFactory {
  friend class MyValue;

public:
  static MyValueFactory &getFactory() {
    static MyValueFactory instance;
    return instance;
  }

private:
  MyValueFactory() {}
  ~MyValueFactory();

  MyIntValue *getIntValue(int num);
  MyRefValue *getRefValue(MyValue *baseValue);
  MyFunValue *getFunValue(std::string name);
  MyTupleValue *getTupleValue(const std::vector<MyValue*> &values);
  MyTupleValue *getUnitValue() {
    return getTupleValue(std::vector<MyValue*>());
  }

private:
  static MyValueFactory *factoryInstance;
  std::vector<MyIntValue*> intValues;
  std::vector<MyRefValue*> refValues;
  std::vector<MyFunValue*> funValues;
  std::vector<MyTupleValue*> tupleValues;
};

}

#endif
