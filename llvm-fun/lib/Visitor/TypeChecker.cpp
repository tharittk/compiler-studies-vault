#include "Visitor/TypeChecker.h"
#include "AST/AST.h"
#include <assert.h>
#include <sstream>

using namespace fun;

// TODO Implement bodies of functions below. You can also add new functions.

bool TypeChecker::sub(MyType *t1, MyType *t2) { return false; }

MyType *TypeChecker::join(MyType *t1, MyType *t2) { return NULL; }


MyType *TypeChecker::visit(CallExpAST *n) { return NULL; }


// Rule 1:
MyType *TypeChecker::visit(IdExpAST *n) {
  if (!ctxt.has(n->getName())) {
    reportError(n->getSrcLoc(), "IdExp: Id name is not bounded");
  }
  // simply look up in the type table
  return ctxt.get(n->getName());
}

// Rule 2:
MyType *TypeChecker::visit(IntExpAST *) { return MyType::getIntType(); }

// Rule 3:
MyType *TypeChecker::visit(SeqExpAST *n) {
  // exp1 does not cause side-effect to the environment ?
  n->getExp1AST()->accept(*this);
  auto exp2Ty = n->getExp2AST()->accept(*this);
  return exp2Ty;
}

// Rule 4:
MyType *TypeChecker::visit(UnExpAST *n) {
  auto expTy = n->getExp1AST()->accept(*this);

  switch (n->getOp()){
    case OpKind::OP_UMinus:{
      if (!MyType::equals(expTy, MyType::getIntType())){
        reportError(n->getSrcLoc(), "Unary Op error: UMinus must be applied to Int");
      }
      return expTy;
    }
    case OpKind::OP_Not: {
      // TODO: Not only applies to Int ?
      if (!MyType::equals(expTy, MyType::getIntType())){
        reportError(n->getSrcLoc(), "Unary Op error: UMinus must be applied to Int");
      }
      return expTy;
    }
    // Rule 13:
    case OpKind::OP_Ref: {
      // takes tp -> tp ref
      return MyType::getRefType(expTy);
    }
    // Rule 14:
    case OpKind::OP_Get: {
      if (!expTy->isRefType()){
        reportError(n->getSrcLoc(), "OP_Get Error, argument must be tp ref");
      }
      return MyType::toRefType(expTy)->getBaseType();
    }

    default:
      reportError(n->getSrcLoc(), "Unary Op error: case does not match");
      return NULL;
  }
}

// Rule 5:
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
    // Rule 15:
    auto leftTyRef = MyType::toRefType(leftTy);
    if (leftTyRef == nullptr){
      reportError(n->getSrcLoc(), "OP_SET :=,leftTy must be tp ref");
    }
    if (!MyType::equals(leftTyRef->getBaseType(), rightTy)) {
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

// Rule 6:
MyType *TypeChecker::visit(TupleExpAST *n) {
  std::vector<MyType *> tupleTy;
  for (const auto exp : n->getExpASTs()){
    auto expTy = exp->accept(*this);
    tupleTy.push_back(expTy);
  }
  return MyType::getTupleType(tupleTy);
}
MyType *TypeChecker::visit(IfExpAST *n) { return NULL; }

// Rule 7:
MyType *TypeChecker::visit(ProjExpAST *n) {
  auto targetTupleTy = n->getTargetTupleExpAST()->accept(*this);
  auto tupleTy = MyType::toTupleType(targetTupleTy);
  if (tupleTy != nullptr){
    if (n->getIndex() >= tupleTy->getLength())
      reportError(n->getSrcLoc(), "Proj error. Index >= tuple list size");
    return tupleTy->getType(n->getIndex());
  }
  reportError(n->getSrcLoc(), "Project error, dyn_cast fails");
  return NULL;
}


// Rule 11:
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

// Rule 12:
MyType *TypeChecker::visit(LetExpAST *n) {
  auto varName = n->getVarName();
  auto varExpTy = n->getVarExpAST()->accept(*this);
  auto ck = ctxt.checkPoint();
  ctxt.bind(varName, varExpTy);
  auto bodyTy = n->getBodyExpAST()->accept(*this);
  ctxt.restoreCheckPoint(ck);
  return bodyTy;
}

// Rule 16
MyType *TypeChecker::visit(ConstrainExpAST *n) {
  auto expTy = n->getExpAST()->accept(*this);
  if (!MyType::equals(expTy, MyType::getType(n->getTypeAST()))){
    reportError(n->getSrcLoc(), "Constrain Error: Exp and tp does not match");
  }
  return expTy;
}

// Rule Function Declaration
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


// Rule Program Typing
MyType *TypeChecker::visit(ProgramAST *n) {
  // Traverse each function declartion in a DFS manner
  const std::map<std::string, FunDeclAST *> &funDecls = n->getFunDeclASTs();
  for (auto it = funDecls.begin(); it != funDecls.end(); ++it) {
    // std::cout << "DFS funcdel name: " << it->second->getName() << "\n";
    it->second->accept(*this);
  }
  return NULL;
}