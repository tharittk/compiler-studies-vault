#ifndef FUN_AST_TYPELIST_H
#define FUN_AST_TYPELIST_H

#include <vector>

namespace fun {

// Intermediary class to pass a type list to TupleTypeAST class
class TypeASTList {
public:
  void append(TypeAST *type) { types.push_back(type); }
  const std::vector<TypeAST*> &getTypeASTList() const { return types; }

private:
  // We are not responsible for deleting these TypeAST* instances in this class's
  // destructor. They will be deleted in TupleTypeAST destructor
  std::vector<TypeAST*> types;
};

}

#endif
