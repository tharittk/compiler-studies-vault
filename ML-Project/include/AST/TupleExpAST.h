#ifndef FUN_AST_TUPLEEXPAST_H
#define FUN_AST_TUPLEEXPAST_H

#include "ExpAST.h"
#include <vector>

namespace llvm { class Value; }

namespace fun {

class ExpASTList;
class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// <exp, ..., exp>
class TupleExpAST : public ExpAST {
public:
  TupleExpAST(SrcLoc srcLoc, ExpASTList *list);
  TupleExpAST(SrcLoc srcLoc, std::vector<ExpAST*> &exps);
  ~TupleExpAST();

  int getLength() const { return exps.size(); }
  ExpAST *getExpAST(int idx) const { return exps[idx]; }
  const std::vector<ExpAST*> &getExpASTs() const { return exps; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Tuple; }

private:
  std::vector<ExpAST*> exps;
};

}

#endif
