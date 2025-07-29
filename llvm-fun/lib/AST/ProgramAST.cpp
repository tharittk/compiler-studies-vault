#include "AST/ProgramAST.h"
#include "AST/FunDeclAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

ProgramAST::~ProgramAST() {
  for (auto &kv : funDecls)
    delete kv.second;
}

void ProgramAST::append(FunDeclAST *funDecl) {
  funDecl->setParent(this);
  funDecls[funDecl->getName()] = funDecl;
}

void ProgramAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *ProgramAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *ProgramAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *ProgramAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
