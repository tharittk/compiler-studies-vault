#include "Visitor/TypeChecker.h"
#include "AST/AST.h"
#include <assert.h>
#include <sstream>

using namespace fun;

// TODO Implement bodies of functions below. You can also add new functions.

bool TypeChecker::sub(MyType *t1, MyType *t2) {
  return false;
}

MyType *TypeChecker::join(MyType *t1, MyType *t2) {
  return NULL;
}

MyType *TypeChecker::visit(BinExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(CallExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(ConstrainExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(FunDeclAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(IdExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(IfExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(IntExpAST *) {
  return NULL;
}

MyType *TypeChecker::visit(LetExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(ProgramAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(ProjExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(SeqExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(TupleExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(UnExpAST *n) {
  return NULL;
}

MyType *TypeChecker::visit(WhileExpAST *n) {
  return NULL;
}
