#ifndef FUN_AST_PROJEXPAST_H
#define FUN_AST_PROJEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// #i <exp, ..., exp>
class ProjExpAST : public ExpAST {
public:
  ProjExpAST(SrcLoc srcLoc, int index, ExpAST *targetTupleExp);
  ~ProjExpAST();

  int getIndex() const { return index; }
  ExpAST *getTargetTupleExpAST() { return targetTupleExp; }
  const ExpAST *getTargetTupleExpAST() const { return targetTupleExp; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Proj; }

private:
  int index;
  ExpAST *targetTupleExp;
};

}

#endif
