#ifndef FUN_AST_INTEXPAST_H
#define FUN_AST_INTEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// num
class IntExpAST : public ExpAST {
public:
  IntExpAST(SrcLoc srcLoc, int num) : ExpAST(OP_Int, srcLoc), num(num) {}

  int getNum() const { return num; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Int; }

private:
  int num;
};

}

#endif
