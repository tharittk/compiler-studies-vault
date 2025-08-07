#ifndef FUN_CODEGEN_LLVMGENERATOR_H
#define FUN_CODEGEN_LLVMGENERATOR_H

#include "AST/SrcLoc.h"
#include "Util/Context.h"
#include "Visitor/LLVMVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include <assert.h>

namespace fun {

using namespace llvm;

class LLVMGenerator : public LLVMVisitor {
public:
  LLVMGenerator(ProgramAST *prg, std::string srcFileName)
    : prg(prg), srcFileName(srcFileName), builder(getGlobalContext()) {}

  Value *visit(BinExpAST *n);
  Value *visit(CallExpAST *n);
  Value *visit(ConstrainExpAST *n);
  Value *visit(FunDeclAST *n);
  Value *visit(FunTypeAST *) { return NULL; }
  Value *visit(IdExpAST *n);
  Value *visit(IfExpAST *n);
  Value *visit(IntExpAST *);
  Value *visit(IntTypeAST *) { return NULL; }
  Value *visit(LetExpAST *n);
  Value *visit(ProgramAST *n);
  Value *visit(ProjExpAST *n);
  Value *visit(RefTypeAST *) { return NULL; }
  Value *visit(SeqExpAST *n);
  Value *visit(TupleExpAST *n);
  Value *visit(TupleTypeAST *) { return NULL; }
  Value *visit(UnExpAST *n);
  Value *visit(WhileExpAST *n);

  void run(std::ostream &out);

private:
  // TODO You can add new helper functions or members here if you need.
  Type* codegenType(MyType* t);

  void reportErrorAndExit(SrcLoc srcLoc, std::string msg) const {
    std::cerr << srcFileName << ":" << srcLoc.line << ":" << srcLoc.col
              << ": error: " << msg << "\n";
    assert(false);
  }

  // Allocate memory on heap based on the size of the given type.
  // Created instructions are inserted at the end of the given basic block.
  Value *allocHeap(Type *t, BasicBlock *bb, std::string name="");

  // TODO Add any helper functions you want to add

  ProgramAST *prg;
  std::string srcFileName;
  Context<std::string, Value*> ctxt;

  IRBuilder<> builder;
  Module *module;
};

}

#endif
