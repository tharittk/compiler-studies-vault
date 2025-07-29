#ifndef FUN_AST_WHILEEXPAST_H
#define FUN_AST_WHILEEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// while exp do exp
class WhileExpAST : public ExpAST {
public:
  WhileExpAST(SrcLoc srcLoc, ExpAST *condExp, ExpAST *bodyExp);
  ~WhileExpAST();

  ExpAST *getCondExpAST() { return condExp; }
  ExpAST *getBodyExpAST() { return bodyExp; }
  const ExpAST *getCondExpAST() const { return condExp; }
  const ExpAST *getBodyExpAST() const { return bodyExp; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_While; }

private:
  ExpAST *condExp;
  ExpAST *bodyExp;
};

}

#endif
