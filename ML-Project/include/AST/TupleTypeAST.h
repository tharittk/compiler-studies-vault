#ifndef FUN_AST_TUPLETYPEAST_H
#define FUN_AST_TUPLETYPEAST_H

#include "TypeAST.h"
#include <vector>

namespace llvm { class Value; }

namespace fun {

class MyValue;
class TypeASTList;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// <tp, ..., tp>
class TupleTypeAST : public TypeAST {
public:
  TupleTypeAST(SrcLoc srcLoc, TypeASTList *list);
  TupleTypeAST(SrcLoc srcLoc, std::vector<TypeAST*> &types);
  ~TupleTypeAST();

  int getLength() const { return types.size(); }
  TypeAST *getTypeAST(int idx) const { return types[idx]; }
  const std::vector<TypeAST*> &getTypeASTs() const { return types; }

  // Returns a matching MyType instance
  virtual MyTupleType *getType() const;

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_TupleType; }

private:
  std::vector<TypeAST*> types;
};

}

#endif
