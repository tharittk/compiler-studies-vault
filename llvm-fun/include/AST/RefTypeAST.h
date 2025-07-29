#ifndef FUN_AST_REFTYPEAST_H
#define FUN_AST_REFTYPEAST_H

#include "TypeAST.h"

namespace llvm { class Value; }

namespace fun {

class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// tp ref
class RefTypeAST : public TypeAST {
public:
  RefTypeAST(SrcLoc srcLoc, TypeAST *baseType);
  ~RefTypeAST();

  TypeAST *getBaseTypeAST() { return baseType; }
  const TypeAST *getBaseTypeAST() const { return baseType; }

  // Returns a matching MyType instance
  virtual MyRefType *getType() const;

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_RefType; }

private:
  TypeAST *baseType;
};

}

#endif
