#ifndef FUN_AST_LETEXPAST_H
#define FUN_AST_LETEXPAST_H

#include "ExpAST.h"

namespace llvm { class Value; }

namespace fun {

class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// let id = exp in exp
class LetExpAST : public ExpAST {
public:
  LetExpAST(SrcLoc srcLoc, std::string varName, ExpAST *varExp,
            ExpAST *bodyExp);
  ~LetExpAST();

  std::string getVarName() const { return varName; }
  ExpAST *getVarExpAST() { return varExp; }
  ExpAST *getBodyExpAST() { return bodyExp; }
  const ExpAST *getVarExpAST() const { return varExp; }
  const ExpAST *getBodyExpAST() const { return bodyExp; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Let; }

private:
  std::string varName;
  ExpAST *varExp;
  ExpAST *bodyExp;
};

}

#endif
