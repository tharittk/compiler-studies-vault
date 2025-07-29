#ifndef FUN_AST_UNEXPAST_H
#define FUN_AST_UNEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// un exp
// un ::= - | not | ! | ref
// (# is represented by ProjExpAST class)
class UnExpAST : public ExpAST {
public:
  UnExpAST(OpKind op, SrcLoc srcLoc, ExpAST *exp1);
  ~UnExpAST();

  ExpAST *getExp1AST() { return exp1; }
  const ExpAST *getExp1AST() const { return exp1; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n);

private:
  ExpAST *exp1;
};

}

#endif
