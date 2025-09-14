#include "AST/OpInfo.h"
#include <assert.h>

using namespace fun;

const std::map<OpKind, std::string> OpInfo::strMap = OpInfo::createStrMap();
const std::map<OpKind, int> OpInfo::precMap = OpInfo::createPrecMap();
const std::map<OpKind, OpAssoc> OpInfo::assocMap = OpInfo::createAssocMap();

std::string OpInfo::getStr(OpKind op) {
  if (strMap.find(op) == strMap.end())
    assert(false && "Only unary and binary operators are supported");
  return strMap.at(op);
}

std::map<OpKind, std::string> OpInfo::createStrMap() {
  std::map<OpKind, std::string> m;
  m[OP_Ref] = "ref ";
  m[OP_RefType] = " ref";
  m[OP_FunType] = "->";
  m[OP_Get] = "!";
  m[OP_UMinus] = "-";
  m[OP_Mul] = "*";
  m[OP_Add] = "+";
  m[OP_Sub] = "-";
  m[OP_Equal] = "=";
  m[OP_LT] = "<";
  m[OP_Not] = "not ";
  m[OP_And] = "&";
  m[OP_Or] = "||";
  m[OP_Constrain] = ":";
  m[OP_Set] = ":=";
  m[OP_Seq] = ";";
  return m;
}

std::map<OpKind, int> OpInfo::createPrecMap() {
  // According to the Definition of Fun language,
  // operator precedence levels are as follows (1 is tightest binding):
  // 1. #i, ref, function call, !, unary minus
  // 2. *
  // 3. +, binary minus
  // 4. =, <
  // 5. not
  // 6. &, ||
  // 7. :
  // 8. :=
  // 9. if-then-else, do-while
  // 10. ;
  // 11. let-in

  std::map<OpKind, int> m;
  m[OP_Proj] = 1;
  m[OP_Ref] = 1;
  m[OP_RefType] = 1;
  m[OP_Call] = 1;
  m[OP_Get] = 1;
  m[OP_UMinus] = 1;
  // The ref constructor has higher precedence than the function type
  // constructor (->). So we give it a lower precedence that 1.
  m[OP_FunType] = 2;
  m[OP_Mul] = 2;
  m[OP_Add] = 3;
  m[OP_Sub] = 3;
  m[OP_Equal] = 4;
  m[OP_LT] = 4;
  m[OP_Not] = 5;
  m[OP_And] = 6;
  m[OP_Or] = 6;
  m[OP_Constrain] = 7;
  m[OP_Set] = 8;
  // The precendences of if-then-else and if-then are the same in the spec,
  // but to solve the 'dangling else' problem, we give a higher precedence
  // to if-then-else. This means 'else' is bound to the closest if.
  // ex) if exp then exp if exp then exp else exp
  //     = if exp then exp (if exp then exp else exp)
  m[OP_IfThenElse] = 9;
  m[OP_IfThen] = 10;
  m[OP_While] = 10;
  m[OP_Seq] = 11;
  m[OP_Let] = 12;

  // These are not strictly operators, so we give them the lowest precedence
  m[OP_Program] = 100;
  m[OP_Int] = 100;
  m[OP_Id] = 100;
  m[OP_Tuple] = 100;
  m[OP_FunDecl] = 100;
  m[OP_IntType] = 100;
  m[OP_TupleType] = 100;

  return m;
}

std::map<OpKind, OpAssoc> OpInfo::createAssocMap() {
  std::map<OpKind, OpAssoc> m;

  m[OP_Proj] = OA_UnaryRight;
  m[OP_Ref] = OA_UnaryRight;
  m[OP_RefType] = OA_UnaryLeft;
  m[OP_Call] = OA_BinaryLeft;
  m[OP_FunType] = OA_BinaryRight;
  m[OP_Get] = OA_UnaryRight;
  m[OP_UMinus] = OA_UnaryRight;
  m[OP_Mul] = OA_BinaryLeft;
  m[OP_Add] = OA_BinaryLeft;
  m[OP_Sub] = OA_BinaryLeft;
  m[OP_Equal] = OA_BinaryLeft;
  m[OP_LT] = OA_BinaryLeft;
  m[OP_Not] = OA_UnaryRight;
  m[OP_And] = OA_BinaryLeft;
  m[OP_Or] = OA_BinaryLeft;
  m[OP_Constrain] = OA_BinaryLeft;
  m[OP_Set] = OA_None;
  m[OP_IfThenElse] = OA_None;
  m[OP_IfThen] = OA_None;
  m[OP_While] = OA_None;
  m[OP_Seq] = OA_BinaryLeft;
  m[OP_Let] = OA_None;

  // These are not strictly operators
  m[OP_Program] = OA_None;
  m[OP_Int] = OA_None;
  m[OP_Id] = OA_None;
  m[OP_Tuple] = OA_None;
  m[OP_FunDecl] = OA_None;
  m[OP_IntType] = OA_None;
  m[OP_TupleType] = OA_None;
  return m;
}

bool OpInfo::isUnaryOp(OpKind op) {
  switch (op) {
  case OP_Ref:
  case OP_RefType:
  case OP_Get:
  case OP_UMinus:
  case OP_Not:
    return true;
  default:
    return false;
  }
}

bool OpInfo::isBinaryOp(OpKind op) {
  switch (op) {
  case OP_FunType:
  case OP_Mul:
  case OP_Add:
  case OP_Sub:
  case OP_Equal:
  case OP_LT:
  case OP_And:
  case OP_Or:
  case OP_Constrain:
  case OP_Set:
  case OP_Seq:
    return true;
  default:
    return false;
  }
}
