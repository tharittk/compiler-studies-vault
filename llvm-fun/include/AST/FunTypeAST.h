#ifndef FUN_AST_FUNTYPEAST_H
#define FUN_AST_FUNTYPEAST_H

#include "TypeAST.h"

namespace llvm { class Value; }

namespace fun {

class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// tp -> tp
class FunTypeAST : public TypeAST {
public:
  FunTypeAST(SrcLoc srcLoc, TypeAST *paramType, TypeAST *retType);
  ~FunTypeAST();

  TypeAST *getParamTypeAST() { return paramType; }
  TypeAST *getRetTypeAST() { return retType; }
  const TypeAST *getParamTypeAST() const { return paramType; }
  const TypeAST *getRetTypeAST() const { return retType; }

  virtual bool isLeftChild(const BaseAST *n) const {
    return paramType == n;
  }
  virtual bool isRightChild(const BaseAST *n) const {
    return retType == n;
  }

  // Returns a matching MyType instance
  virtual MyFunType *getType() const;

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_FunType; }

private:
  TypeAST *paramType;
  TypeAST *retType;
};

}

#endif
