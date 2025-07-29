#ifndef FUN_AST_INTTYPEAST_H
#define FUN_AST_INTTYPEAST_H

#include "TypeAST.h"

namespace llvm { class Value; }

namespace fun {

class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// int
class IntTypeAST : public TypeAST {
public:
  IntTypeAST(SrcLoc srcLoc) : TypeAST(OP_IntType, srcLoc) {}

  // Returns a matching MyType instance
  virtual MyIntType *getType() const;

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_IntType; }
};

}

#endif
