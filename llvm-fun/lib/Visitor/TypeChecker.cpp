#include "Visitor/TypeChecker.h"
#include "AST/AST.h"
#include <assert.h>
#include <sstream>

using namespace fun;

bool TypeChecker::sub(MyType *t1, MyType *t2) { return false; }

MyType *TypeChecker::join(MyType *t1, MyType *t2) {
  // TODO (tharitt): make it mutually recursive to find Least-common supertype
  if (MyType::equals(t1, t2)){
    return t1;
  } else {
    std::cerr << "Join error (TO IMPLEMENTED): t1 != t2";
    return t1;
  }
}

// Rule 1:
MyType *TypeChecker::visit(IdExpAST *n) {
  if (!ctxt.has(n->getName())) {
    // std::cerr << "IdExp: Id name is not bounded: " << n->getName();
    auto msg = "IdExp Error: "+ n->getName() + " is not in env";
    reportError(n->getSrcLoc(), msg);
    return MyType::getIntType();
  }
  return ctxt.get(n->getName());
}

// Rule 2:
MyType *TypeChecker::visit(IntExpAST *) { return MyType::getIntType(); }

// Rule 3:
MyType *TypeChecker::visit(SeqExpAST *n) {
  // exp1 may change environment (ctxt)
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
        reportError(n->getSrcLoc(), "UnExp Error: UMinus must be applied to Int");
        return MyType::getIntType();
      }
      return expTy;
    }
    case OpKind::OP_Not: {
      if (!MyType::equals(expTy, MyType::getIntType())){
        reportError(n->getSrcLoc(), "UnExp Error: NOT must be applied to Int");
        return MyType::getIntType();
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
        reportError(n->getSrcLoc(), "UnExp Error: GET must be applied to RefType");
        return MyType::getIntType();
      }
      return MyType::toRefType(expTy)->getBaseType();
    }
    default:
      reportError(n->getSrcLoc(), "UnExp Error: case does not match");
      return MyType::getIntType();
  }
}

// Rule 5:
MyType *TypeChecker::visit(BinExpAST *n) {
  auto leftExp = n->getExp1AST();
  auto rightExp = n->getExp2AST();
  auto leftTy = leftExp->accept(*this);
  auto rightTy = rightExp->accept(*this);
  switch (n->getOp()) {
  case OpKind::OP_Set: {
    // Rule 15:
    auto leftTyRef = MyType::toRefType(leftTy);
    if (leftTyRef == nullptr){
      reportError(n->getSrcLoc(), "BinExp Error: LeftTy is null");
    }
    if (!MyType::equals(leftTyRef->getBaseType(), rightTy)) {
      reportError(n->getSrcLoc(),
                  "BinExp Error: SET BaseType of Left must be the same as Right Type");
    }
    return MyType::getUnitType();
  }
  default: {
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

MyType *TypeChecker::visit(IfExpAST *n) {
  auto condTy = n->getCondExpAST()->accept(*this);
  if (!MyType::equals(condTy, MyType::getIntType())){
    reportError(n->getSrcLoc(), "IfExp Error: condTy should be Int");
    return MyType::getIntType();
  }
  auto thenTy = n->getThenExpAST()->accept(*this);
  // Rule 10:
  if (n->getElseExpAST() == nullptr){
    if (!MyType::equals(thenTy, MyType::getUnitType())){
      reportError(n->getSrcLoc(), "IfExp Error: THEN should have UnitTy (case: no ELSE)");
    }
    return MyType::getUnitType();
  }
  // Rule 9: TODO (tharitt): need to do join (least super type)
  auto elseTy = n->getElseExpAST()->accept(*this);
  return join(thenTy, elseTy);
}

// Rule 7:
MyType *TypeChecker::visit(ProjExpAST *n) {
  auto targetTupleTy = n->getTargetTupleExpAST()->accept(*this);
  auto tupleTy = MyType::toTupleType(targetTupleTy);
  if (tupleTy != nullptr){
    if (n->getIndex() >= tupleTy->getLength()){
      reportError(n->getSrcLoc(), "ProjExp Error: Index >= tuple list size");
      return MyType::getIntType();
    }
    return tupleTy->getType(n->getIndex());
  }
  reportError(n->getSrcLoc(), "ProjExp Error: target is not Tuple Type");
  return MyType::getIntType();
}

// Rule 8;
MyType *TypeChecker::visit(CallExpAST *n) {
  auto funTy = n->getFunExpAST()->accept(*this);
  auto argTy = n->getArgExpAST()->accept(*this);
  if (!funTy->isFunType()){
    reportError(n->getSrcLoc(), "CallExp Error: exp is not a function type");
    return MyType::getIntType();
  }
  // check that supplied arg type = declared arg type
  auto declTy = MyType::toFunType(funTy)->getParamType();
  if (!MyType::equals(argTy, declTy)){
    reportError(n->getSrcLoc(), "CallExp Error: argTy and declTy mistmatches");
    return MyType::getIntType();
  }
  return MyType::toFunType(funTy)->getRetType();
}

// Rule 11:
MyType *TypeChecker::visit(WhileExpAST *n) {
  auto condExp = n->getCondExpAST();
  auto bodyExp = n->getBodyExpAST();
  auto condTy = condExp->accept(*this);
  auto bodyTy = bodyExp->accept(*this);
  if (!MyType::equals(condTy, MyType::getIntType())) {
    reportError(n->getSrcLoc(), "WhileExp Error: Cond type must be int");
    return MyType::getIntType();
  }
  if (!MyType::equals(bodyTy, MyType::getUnitType())) {
    reportError(n->getSrcLoc(), "WhileExp Error: body type must be unit");
    return MyType::getIntType();
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
    reportError(n->getSrcLoc(), "ConstrainExp Error: Exp and tp does not match");
    return MyType::getIntType();
  }
  return expTy;
}

// Rule Function Declaration
MyType *TypeChecker::visit(FunDeclAST *n) {
  auto retTy = MyType::getType(n->getRetTypeAST());
  auto paramTy = MyType::getType(n->getParamTypeAST());
  auto funTy = MyType::getFunType(paramTy, retTy);
  ctxt.bind(n->getName(), funTy);
  ctxt.bind(n->getParamName(), paramTy); // for body checking
  auto body_ty = n->getBodyExpAST()->accept(*this);
  if (!MyType::equals(body_ty, retTy)) {
    reportError(n->getSrcLoc(), "FunDeclExp Error: body vs. ret type mistmatches");
    return MyType::getIntType();
  }
  ctxt.undoOne(); // clean up the param
  return funTy;
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