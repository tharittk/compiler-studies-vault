#ifndef FUN_AST_EXPAST_H
#define FUN_AST_EXPAST_H

#include "BaseAST.h"

namespace fun {

// Base class for all expression
class ExpAST : public BaseAST {
public:
  ExpAST(OpKind op, SrcLoc srcLoc) : BaseAST(op, srcLoc) {}
};

}

#endif
