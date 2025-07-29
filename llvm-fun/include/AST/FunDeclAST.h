#ifndef FUN_AST_FUNDECLAST_H
#define FUN_AST_FUNDECLAST_H

#include "BaseAST.h"
#include <string>

namespace llvm { class Value; }

namespace fun {

class ExpAST;
class TypeAST;
class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// fun id ( id : tp ) : tp = exp
class FunDeclAST : public BaseAST {
public:
  FunDeclAST(SrcLoc srcLoc, std::string name, std::string paramName,
             TypeAST *paramType, TypeAST *retType,
             ExpAST *bodyExp);
  ~FunDeclAST();

  std::string getName() const { return name; }
  std::string getParamName() const { return paramName; }
  TypeAST *getParamTypeAST() { return paramType; }
  TypeAST *getRetTypeAST() { return retType; }
  ExpAST *getBodyExpAST() { return bodyExp; }
  const TypeAST *getParamTypeAST() const { return paramType; }
  const TypeAST *getRetTypeAST() const { return retType; }
  const ExpAST *getBodyExpAST() const { return bodyExp; }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_FunDecl; }

private:
  std::string name;
  std::string paramName;
  TypeAST *paramType;
  TypeAST *retType;
  ExpAST *bodyExp;
};

}

#endif
