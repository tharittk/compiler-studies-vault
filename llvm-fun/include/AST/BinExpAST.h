#ifndef FUN_AST_BINEXPAST_H
#define FUN_AST_BINEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// exp bin exp
// bin ::= + | - | * | & | || | = | < | :=
class BinExpAST : public ExpAST {
public:
  BinExpAST(OpKind op, SrcLoc srcLoc, ExpAST *exp1, ExpAST *exp2);
  ~BinExpAST();

  ExpAST *getExp1AST() { return exp1; }
  ExpAST *getExp2AST() { return exp2; }
  const ExpAST *getExp1AST() const { return exp1; }
  const ExpAST *getExp2AST() const { return exp2; }

  virtual bool isLeftChild(const BaseAST *n) const { return exp1 == n; }
  virtual bool isRightChild(const BaseAST *n) const { return exp2 == n; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n);

private:
  ExpAST *exp1;
  ExpAST *exp2;
};

}

#endif
