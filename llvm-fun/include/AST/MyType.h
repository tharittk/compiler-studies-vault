#ifndef FUN_AST_MYTYPE_H
#define FUN_AST_MYTYPE_H

#include <sstream>
#include <string>
#include <vector>

namespace fun {

class TypeAST;
class MyIntType;
class MyRefType;
class MyFunType;
class MyTupleType;
class MyTypeFactory;

// Base class for all types.
class MyType {
  friend class MyTypeFactory;

public:
  // Use these methods to get instances of type subclasses; you are not allowed
  // to instantiate subclasses of MyType class directly by calling 'new'.
  static MyIntType *getIntType();
  static MyRefType *getRefType(MyType *baseType);
  static MyFunType *getFunType(MyType *paramType, MyType *retType);
  static MyTupleType *getTupleType(const std::vector<MyType *> &types);
  static MyTupleType *getUnitType(); // empty tuple

  // Downcasts the given MyType class pointer into one of its child class
  // pointers. Returns null if the casting cannot be done.
  static MyIntType *toIntType(MyType *t);
  static MyRefType *toRefType(MyType *t);
  static MyFunType *toFunType(MyType *t);
  static MyTupleType *toTupleType(MyType *t);

  // Returns true if two type instances are semantically equal
  static bool equals(const MyType *t1, const MyType *t2) { return t1 == t2; }

  // Returns an instance of the matching type class with given TypeAST node.
  // For example, if an IntTypeAST instance is given, this will return an
  // instance of MyIntType.
  static MyType *getType(TypeAST *tn);

  // Returns string representation of this type
  virtual std::string toString() const = 0;

  // Type testing
  virtual bool isIntType() const { return false; }
  virtual bool isRefType() const { return false; }
  virtual bool isFunType() const { return false; }
  virtual bool isTupleType() const { return false; }

  // Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
  enum TypeKind { TK_Int, TK_Ref, TK_Fun, TK_Tuple };
  TypeKind getKind() const { return kind; }

protected:
  explicit MyType(TypeKind k) : kind(k) {}
  virtual ~MyType() {}

private:
  static MyTypeFactory &factory;
  const TypeKind kind;
};

class MyIntType : public MyType {
  friend class MyTypeFactory;

public:
  virtual bool isIntType() const { return true; }
  virtual std::string toString() const { return "int"; }

  // For LLVM-style RTTI support
  static bool classof(const MyType *t) { return t->getKind() == TK_Int; }

private:
  MyIntType() : MyType(TK_Int) {}
};

class MyRefType : public MyType {
  friend class MyTypeFactory;

public:
  MyType *getBaseType() { return baseType; }
  virtual bool isRefType() const { return true; }

  virtual std::string toString() const {
    std::stringstream ss;
    if (baseType->isFunType())
      ss << "(";
    ss << baseType->toString();
    if (baseType->isFunType())
      ss << ")";
    ss << " ref";
    return ss.str();
  }

  // For LLVM-style RTTI support
  static bool classof(const MyType *t) { return t->getKind() == TK_Ref; }

private:
  explicit MyRefType(MyType *baseType) : MyType(TK_Ref), baseType(baseType) {}
  MyType *baseType;
};

class MyFunType : public MyType {
  friend class MyTypeFactory;

public:
  MyType *getParamType() { return paramType; }
  MyType *getRetType() { return retType; }
  virtual bool isFunType() const { return true; }

  virtual std::string toString() const {
    std::stringstream ss;
    // We should put parentheses in the case of (x->y)->z
    if (paramType->isFunType())
      ss << "(";
    ss << paramType->toString();
    if (paramType->isFunType())
      ss << ")";
    ss << "->";
    ss << retType->toString();
    return ss.str();
  }

  // For LLVM-style RTTI support
  static bool classof(const MyType *t) { return t->getKind() == TK_Fun; }

private:
  MyFunType(MyType *paramType, MyType *retType)
      : MyType(TK_Fun), paramType(paramType), retType(retType) {}
  MyType *paramType;
  MyType *retType;
};

class MyTupleType : public MyType {
  friend class MyTypeFactory;

public:
  int getLength() const { return types.size(); }
  MyType *getType(int idx) { return types[idx]; }
  const std::vector<MyType *> &getTypes() const { return types; }
  virtual bool isTupleType() const { return true; }

  virtual std::string toString() const;

  // For LLVM-style RTTI support
  static bool classof(const MyType *t) { return t->getKind() == TK_Tuple; }

private:
  explicit MyTupleType(const std::vector<MyType *> &types)
      : MyType(TK_Tuple), types(types) {}
  std::vector<MyType *> types;
};

class MyTypeFactory {
  friend class MyType;

public:
  static MyTypeFactory &getFactory() {
    static MyTypeFactory instance;
    return instance;
  }

private:
  MyTypeFactory() {}
  ~MyTypeFactory();

  MyIntType *getIntType() { return &intType; }
  MyRefType *getRefType(MyType *baseType);
  MyFunType *getFunType(MyType *paramType, MyType *retType);
  MyTupleType *getTupleType(const std::vector<MyType *> &types);
  MyTupleType *getUnitType() { return getTupleType(std::vector<MyType *>()); }

private:
  static MyTypeFactory *factoryInstance;
  MyIntType intType;
  std::vector<MyRefType *> refTypes;
  std::vector<MyFunType *> funTypes;
  std::vector<MyTupleType *> tupleTypes;
};

} // namespace fun

#endif
