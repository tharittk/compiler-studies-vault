#ifndef FUN_AST_OPINFO_H
#define FUN_AST_OPINFO_H

#include <map>
#include <string>

namespace fun {

enum OpKind {
  OP_Proj,        // #
  OP_Ref,         // ref exp
  OP_RefType,     // type ref
  OP_Call,        // function call
  OP_FunType,     // ->
  OP_Get,         // !
  OP_UMinus,      // unary minus
  OP_Mul,         // *
  OP_Add,         // +
  OP_Sub,         // -
  OP_Equal,       // =
  OP_LT,          // <
  OP_Not,         // not
  OP_And,         // &
  OP_Or,          // ||
  OP_Constrain,   // :
  OP_Set,         // :=
  OP_IfThenElse,  // if-then-else
  OP_IfThen,      // if-then
  OP_While,       // do-while
  OP_Seq,         // ;
  OP_Let,         // let-in

  // Below are not strictly operators, but we have these values anyway to
  // supprot LLVM-style RTTI (isa<>, dyn_cast<>, ...)
  OP_Program,     // program
  OP_Int,         // int
  OP_Id,          // id
  OP_Tuple,       // tuple
  OP_FunDecl,     // function declaration
  OP_IntType,     // int type
  OP_TupleType,   // tuple type
};

enum OpAssoc {
  OA_None,        // No associativity
  OA_UnaryLeft,   // Unary op, left associativity
  OA_UnaryRight,  // Unary op, right associativity
  OA_BinaryLeft,  // Binary op, left associativity
  OA_BinaryRight  // Binary op, right associativity
};

class OpInfo {
public:
  // Returns operator string for unary/binary operators ("+", "-", ...)
  static std::string getStr(OpKind op);
  // Returns operator precedence number. Big number has lower precedence
  static int getPrec(OpKind op) { return precMap.at(op); }
  // Returns operator associativity
  static OpAssoc getAssoc(OpKind op) { return assocMap.at(op); }

  static bool isUnaryOp(OpKind op);
  static bool isBinaryOp(OpKind op);

private:
  static std::map<OpKind, std::string> createStrMap();
  static std::map<OpKind, int> createPrecMap();
  static std::map<OpKind, OpAssoc> createAssocMap();

  // Operator to unary/binary operator string map
  static const std::map<OpKind, std::string> strMap;
  // Operator to precedence map
  static const std::map<OpKind, int> precMap;
  // Operator to associativity map
  static const std::map<OpKind, OpAssoc> assocMap;
};

}

#endif
