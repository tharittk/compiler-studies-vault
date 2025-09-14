#ifndef FUN_AST_IFEXPAST_H
#define FUN_AST_IFEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// if exp then exp else exp | if exp then exp
class IfExpAST : public ExpAST {
public:
  IfExpAST(SrcLoc srcLoc, ExpAST *condExp, ExpAST *thenExp,
           ExpAST *elseExp=NULL);
  ~IfExpAST();

  ExpAST *getCondExpAST() { return condExp; }
  ExpAST *getThenExpAST() { return thenExp; }
  ExpAST *getElseExpAST() { return elseExp; }
  const ExpAST *getCondExpAST() const { return condExp; }
  const ExpAST *getThenExpAST() const { return thenExp; }
  const ExpAST *getElseExpAST() const { return elseExp; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) {
    return n->getOp() == OP_IfThen || n->getOp() == OP_IfThenElse;
  }

private:
  ExpAST *condExp;
  ExpAST *thenExp;
  ExpAST *elseExp;
};

}

#endif
