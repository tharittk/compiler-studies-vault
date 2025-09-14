#include "AST/FunDeclAST.h"
#include "AST/ExpAST.h"
#include "AST/TypeAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

FunDeclAST::FunDeclAST(SrcLoc srcLoc, std::string name, std::string paramName,
                       TypeAST *paramType, TypeAST *retType, ExpAST *bodyExp)
  : BaseAST(OP_FunDecl, srcLoc), name(name), paramName(paramName),
    paramType(paramType), retType(retType), bodyExp(bodyExp) {
  paramType->setParent(this);
  retType->setParent(this);
  bodyExp->setParent(this);
}

FunDeclAST::~FunDeclAST() {
  delete paramType;
  delete retType;
  delete bodyExp;
}

void FunDeclAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *FunDeclAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *FunDeclAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *FunDeclAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
