#ifndef FUN_AST_BASEAST_H
#define FUN_AST_BASEAST_H

#include "OpInfo.h"
#include "SrcLoc.h"
#include "llvm/Support/Casting.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// Base class for all nodes (including program nodes and types)
class BaseAST {
public:
  BaseAST(OpKind op, SrcLoc srcLoc) : parent(NULL), op(op), srcLoc(srcLoc) {}
  virtual ~BaseAST() {}

  void setParent(BaseAST *n) { parent = n; }
  BaseAST *getParent() const { return parent; }

  OpKind getOp() const { return op; }

  std::string getOpStr() const { return OpInfo::getStr(getOp()); }
  int getOpPrec() const { return OpInfo::getPrec(getOp()); }
  OpAssoc getOpAssoc() const { return OpInfo::getAssoc(getOp()); }
  bool isUnaryOp() const { return OpInfo::isUnaryOp(getOp()); }
  bool isBinaryOp() const { return OpInfo::isBinaryOp(getOp()); }

  // For AST structures that have binary operator-like written forms,
  // this returns true if the argument is LeftChild in
  // "LeftChild OP RightChild"
  virtual bool isLeftChild(const BaseAST *) const { return false; }
  // Returns true if the argument is RightChild in "LeftChild OP RightChild"
  virtual bool isRightChild(const BaseAST *) const { return false; }

  SrcLoc getSrcLoc() const { return srcLoc; }
  int getSrcLine() const { return srcLoc.line; }
  int getSrcCol() const { return srcLoc.col; }

  virtual void accept(Visitor &v) = 0;
  virtual MyType *accept(TypeVisitor &v) = 0;
  virtual MyValue *accept(ValueVisitor &v) = 0;
  virtual llvm::Value *accept(LLVMVisitor &v) = 0;

private:
  BaseAST *parent; // Parent node
  const OpKind op;
  SrcLoc srcLoc; // source line and column
};

}

#endif
