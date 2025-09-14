#ifndef FUN_AST_CONSTRAINEXPAST_H
#define FUN_AST_CONSTRAINEXPAST_H

#include "ExpAST.h"
#include "TypeAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// exp : tp
class ConstrainExpAST : public ExpAST {
public:
  ConstrainExpAST(SrcLoc srcLoc, ExpAST *exp, TypeAST *type);
  ~ConstrainExpAST();

  ExpAST *getExpAST() { return exp; }
  TypeAST *getTypeAST() { return type; }
  const ExpAST *getExpAST() const { return exp; }
  const TypeAST *getTypeAST() const { return type; }

  virtual bool isLeftChild(const BaseAST *n) const { return exp == n; }
  virtual bool isRightChild(const BaseAST *n) const { return type == n; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Constrain; }

private:
  ExpAST *exp;
  TypeAST *type;
};

}

#endif
