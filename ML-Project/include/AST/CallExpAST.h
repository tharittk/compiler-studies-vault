#ifndef FUN_AST_CALLEXPAST_H
#define FUN_AST_CALLEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// exp (exp)
class CallExpAST : public ExpAST {
public:
  CallExpAST(SrcLoc srcLoc, ExpAST *funExp, ExpAST *argExp);
  ~CallExpAST();

  ExpAST *getFunExpAST() { return funExp; }
  ExpAST *getArgExpAST() { return argExp; }
  const ExpAST *getFunExpAST() const { return funExp; }
  const ExpAST *getArgExpAST() const { return argExp; }

  virtual bool isLeftChild(const BaseAST *n) const { return funExp == n; }
  virtual bool isRightChild(const BaseAST *n) const { return argExp == n; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Call; }

private:
  ExpAST *funExp;
  ExpAST *argExp;
};

}

#endif
