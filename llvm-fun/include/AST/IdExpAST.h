#ifndef FUN_AST_IDEXPAST_H
#define FUN_AST_IDEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// id
class IdExpAST : public ExpAST {
public:
  IdExpAST(SrcLoc srcLoc, std::string name) : ExpAST(OP_Id, srcLoc), name(name) {}

  std::string getName() const { return name; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Id; }

private:
  std::string name;
};

}

#endif
