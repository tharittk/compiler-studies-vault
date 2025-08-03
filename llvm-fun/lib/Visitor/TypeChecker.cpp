#include "Visitor/TypeChecker.h"
#include "AST/AST.h"
#include <assert.h>
#include <sstream>

using namespace fun;

// TODO Implement bodies of functions below. You can also add new functions.

bool TypeChecker::sub(MyType *t1, MyType *t2) { return false; }

MyType *TypeChecker::join(MyType *t1, MyType *t2) { return NULL; }

MyType *TypeChecker::visit(BinExpAST *n) {
  auto leftExp = n->getExp1AST();
  auto rightExp = n->getExp2AST();

  auto leftTy = leftExp->accept(*this);
  auto rightTy = rightExp->accept(*this);

  switch (n->getOp()) {
  case OpKind::OP_Equal: {
    // may be id = id (var), simply check to type equivalent
    if (!MyType::equals(leftTy, rightTy)) {
      reportError(n->getSrcLoc(),
                  "BinExp: left and right must be equal type in OP_Equal");
    }
    return MyType::getUnitType();
  }
  case OpKind::OP_Set: {
    if (!MyType::equals(leftTy, rightTy)) {
      reportError(n->getSrcLoc(),
                  "BinExp: left and right must be equal type in OP_Set");
    }
    return MyType::getUnitType();
  }
  default: {
    // both argument must be int
    if (!MyType::equals(leftTy, MyType::getIntType())) {
      reportError(n->getSrcLoc(), "BinExp: left must be Int Type");
    }
    if (!MyType::equals(rightTy, MyType::getIntType())) {
      reportError(n->getSrcLoc(), "BinExp: right must be Int Type");
    }
    return MyType::getIntType();
  }
  }
}

MyType *TypeChecker::visit(CallExpAST *n) { return NULL; }

MyType *TypeChecker::visit(ConstrainExpAST *n) { return NULL; }

MyType *TypeChecker::visit(FunDeclAST *n) {
  auto retTy = MyType::getType(n->getRetTypeAST());
  ctxt.bind(n->getName(), retTy);
  auto ck = ctxt.checkPoint();
  // below this only exist inside the function call
  auto paramTy = MyType::getType(n->getParamTypeAST());
  ctxt.bind(n->getParamName(), paramTy);

  auto body_ty = n->getBodyExpAST()->accept(*this);
  if (!MyType::equals(body_ty, MyType::getType(n->getRetTypeAST()))) {
    reportError(n->getSrcLoc(), "FunDecl: body vs. ret mistmatch");
  }
  // undo parameter binding
  ctxt.restoreCheckPoint(ck);
  return MyType::getFunType(paramTy, retTy);
}

MyType *TypeChecker::visit(IdExpAST *n) {
  if (!ctxt.has(n->getName())) {
    reportError(n->getSrcLoc(), "IdExp: Id name is not bounded");
  }
  // simply look up in the type table
  return ctxt.get(n->getName());
}

MyType *TypeChecker::visit(IfExpAST *n) { return NULL; }

MyType *TypeChecker::visit(IntExpAST *) { return MyType::getIntType(); }

MyType *TypeChecker::visit(LetExpAST *n) { return NULL; }

MyType *TypeChecker::visit(ProgramAST *n) {
  // Traverse each function declartion in a DFS manner
  const std::map<std::string, FunDeclAST *> &funDecls = n->getFunDeclASTs();
  for (auto it = funDecls.begin(); it != funDecls.end(); ++it) {
    // std::cout << "DFS funcdel name: " << it->second->getName() << "\n";
    it->second->accept(*this);
  }
  return NULL;
}

MyType *TypeChecker::visit(ProjExpAST *n) { return NULL; }

MyType *TypeChecker::visit(SeqExpAST *n) {
  // exp1 does not cause side-effect to the environment ?
  n->getExp1AST()->accept(*this);
  auto exp2Ty = n->getExp2AST()->accept(*this);

  return exp2Ty;
}

MyType *TypeChecker::visit(TupleExpAST *n) { return NULL; }

MyType *TypeChecker::visit(UnExpAST *n) { return NULL; }

MyType *TypeChecker::visit(WhileExpAST *n) {
  auto condExp = n->getCondExpAST();
  auto bodyExp = n->getBodyExpAST();

  auto condTy = condExp->accept(*this);
  auto bodyTy = bodyExp->accept(*this);

  if (!MyType::equals(condTy, MyType::getIntType())) {
    reportError(n->getSrcLoc(), "WhileExp Error: Cond type must be int");
  }

  if (!MyType::equals(bodyTy, MyType::getUnitType())) {
    reportError(n->getSrcLoc(), "WhileExp Error: body type must be unit");
  }

  return MyType::getUnitType();
}
