#include "AST/LetExpAST.h"
#include "Visitor/Visitors.h"

using namespace fun;

LetExpAST::LetExpAST(SrcLoc srcLoc, std::string varName, ExpAST *varExp,
                     ExpAST *bodyExp)
  : ExpAST(OP_Let, srcLoc), varName(varName), varExp(varExp), bodyExp(bodyExp) {
  varExp->setParent(this);
  bodyExp->setParent(this);
}

LetExpAST::~LetExpAST() {
  delete varExp;
  delete bodyExp;
}

void LetExpAST::accept(Visitor &v) {
  v.visit(this);
}

MyType *LetExpAST::accept(TypeVisitor &v) {
  return v.visit(this);
}

MyValue *LetExpAST::accept(ValueVisitor &v) {
  return v.visit(this);
}

llvm::Value *LetExpAST::accept(LLVMVisitor &v) {
  return v.visit(this);
}
