#ifndef FUN_AST_PROGRAMAST_H
#define FUN_AST_PROGRAMAST_H

#include "BaseAST.h"
#include <map>

namespace llvm { class Value; }

namespace fun {

class FunDeclAST;
class MyType;
class MyValue;
class Visitor;
class TypeVisitor;
class ValueVisitor;
class LLVMVisitor;

// Top-level program
class ProgramAST : public BaseAST {
public:
  ProgramAST(SrcLoc srcLoc) : BaseAST(OP_Program, srcLoc) {}
  ~ProgramAST();
  void append(FunDeclAST *funDecl);

  std::map<std::string, FunDeclAST*> &getFunDeclASTs() { return funDecls; }
  const std::map<std::string, FunDeclAST*> &getFunDeclASTs() const {
    return funDecls;
  }

  FunDeclAST *getFunDeclAST(std::string name) { return funDecls.at(name); }
  const FunDeclAST *getFunDeclAST(std::string name) const {
    return funDecls.at(name);
  }

  virtual void accept(Visitor &v);
  virtual MyType *accept(TypeVisitor &v);
  virtual MyValue *accept(ValueVisitor &v);
  virtual llvm::Value *accept(LLVMVisitor &v);

  // For LLVM-styld RTTI support
  static bool classof(const BaseAST *n) { return n->getOp() == OP_Program; }

private:
  std::map<std::string, FunDeclAST*> funDecls;
};

}

#endif
