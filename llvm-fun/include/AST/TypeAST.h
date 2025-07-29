#ifndef FUN_AST_TYPEAST_H
#define FUN_AST_TYPEAST_H

#include "BaseAST.h"
#include "MyType.h"

namespace fun {

// Base class for all types
class TypeAST : public BaseAST {
public:
  TypeAST(OpKind op, SrcLoc srcLoc) : BaseAST(op, srcLoc) {}

  // Returns a matching MyType instance
  virtual MyType *getType() const = 0;
};

}

#endif
