#ifndef FUN_AST_EXPLIST_H
#define FUN_AST_EXPLIST_H

#include <vector>

namespace fun {

// Intermediary class to pass expression list to TupleExpAST class
class ExpASTList {
public:
  void append(ExpAST *exp) { exps.push_back(exp); }
  const std::vector<ExpAST*> &getExpList() const { return exps; }

private:
  // We are not responsible for deleting these ExpAST* instances in this class's
  // destructor. They will be deleted in TupleExpAST destructor
  std::vector<ExpAST*> exps;
};

}

#endif
