#include "CodeGen/LLVMGenerator.h"
#include "AST/AST.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Verifier.h"

using namespace fun;
using namespace llvm;

void LLVMGenerator::run(std::ostream &out) {
  InitializeNativeTarget();
  module = new Module(srcFileName, getGlobalContext());

  // Actually we don't need JIT feature in this assignment, but we create this
  // to get DataLayout and TargetMachine instances.
  std::string errStr;
  ExecutionEngine *engine = EngineBuilder(module).setErrorStr(&errStr).create();
  if (!engine) {
    std::cerr << "Could not create ExecutionEngine: " << errStr << "\n";
    exit(1);
  }
  module->setDataLayout(engine->getDataLayout());
  module->setTargetTriple(engine->getTargetMachine()->getTargetTriple());

  visit(prg);

  // Print out all of the generated code.
  std::string buf;
  raw_string_ostream os(buf);
  os << *module;
  os.flush();
  out << buf;
  delete engine;
}

Value *LLVMGenerator::allocHeap(Type *t, BasicBlock *bb, std::string name) {
  const DataLayout *layout = module->getDataLayout();
  ConstantInt *allocSize =
    ConstantInt::get(Type::getInt32Ty(getGlobalContext()),
                     layout->getTypeAllocSize(t));

  Instruction *inst = CallInst::CreateMalloc(
      bb,
      Type::getInt32Ty(getGlobalContext()),
      t,
      allocSize,
      nullptr,
      nullptr,
      name);
  bb->getInstList().push_back(inst);
  return inst;
}

Value *LLVMGenerator::visit(BinExpAST *n) {
  auto leftExp = n->getExp1AST();
  auto rightExp = n->getExp2AST();

  Value *L = leftExp->accept(*this);
  Value *R = rightExp->accept(*this);
  if (!L || !R)
    reportErrorAndExit(n->getSrcLoc(), "BinExp Error: L or R Value is null");

  switch (n->getOp()) {
    case OpKind::OP_Add:
      return this->builder.CreateAdd(L, R, "addf");
    case OpKind::OP_Sub:
      return this->builder.CreateSub(L, R, "subf");
    case OpKind::OP_Mul:
      return this->builder.CreateMul(L, R, "mulf");
    case OpKind::OP_Equal:
      return this->builder.CreateICmp(CmpInst::ICMP_EQ, L, R, "eqf");
    case OpKind::OP_LT:
      return this->builder.CreateICmp(CmpInst::ICMP_SLT, L, R, "ltf");
    case OpKind::OP_And:
      return this->builder.CreateAnd(L, R, "andf");
    case OpKind::OP_Or:
      return this->builder.CreateOr(L, R, "orf");
    case OpKind::OP_Set:{
      // auto Int32Ty = Type::getInt32Ty(getGlobalContext());
      // AllocaInst *Ptr = this->builder.CreateAlloca(Int32Ty, L, "ptr");
      return this->builder.CreateStore(R, L);
    }
    default:{
      reportErrorAndExit(n->getSrcLoc(), "BinExp Error: No case matches");
      return nullptr;
    }
  }
}

Value *LLVMGenerator::visit(CallExpAST *n) {
  auto funExp = n->getFunExpAST();
  auto argExp = n->getArgExpAST();

  // simple because our language supports only one variable
  Value* funV = funExp->accept(*this);
  if (!dyn_cast<Function> (funV))
    reportErrorAndExit(n->getSrcLoc(), "CallExp Error: dyn_cast test of funV must be Function Type");
  Function *CalleeF = module->getFunction(funV->getName());

  Value* argV = argExp->accept(*this);
  return builder.CreateCall(CalleeF, argV, "callf");
}

Value *LLVMGenerator::visit(ConstrainExpAST *n) {

}

Value *LLVMGenerator::visit(FunDeclAST *n) {
  // register function to environment
  auto retTy = MyType::getType(n->getRetTypeAST());
  auto paramTy = MyType::getType(n->getParamTypeAST());
  // case swtich, int, ref, fun ,tuple, unit
  FunctionType *FT = FunctionType::get(/*return ty*/codegenType(retTy),
                                  /*arg ty*/codegenType(paramTy), /*vararg*/ false);

  // register to the module for symbol look up
  Function *F = Function::Create(FT, Function::ExternalLinkage, n->getName(), module);

  // llvm semantic, if the symbol already exists, F will be automatically renamed
  // and thus not equal to intended name
  if (F->getName() != n->getName()){
    F->eraseFromParent();
    F = module->getFunction(n->getName());
  }

  // function in our language takes only one argument
  Function::arg_iterator AI = F->arg_begin();
  AI->setName(n->getParamName());

  // no need for this since LLVM uses module "context" for functions
  // ctxt.bind(n->getName(), F);
  // ctxt.bind(n->getParamName(), );
  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
  builder.SetInsertPoint(BB);
  if(Value* retVal = n->getBodyExpAST()->accept(*this)){
    builder.CreateRet(retVal);
    verifyFunction(*F); 
    return F;
  }
}

Value *LLVMGenerator::visit(IdExpAST *n) {
  if (!ctxt.has(n->getName())) {
    auto msg = "IdExp Error: "+ n->getName() + " is not in env";
    reportErrorAndExit(n->getSrcLoc(), msg);
  }
  return ctxt.get(n->getName());
}

Value *LLVMGenerator::visit(IfExpAST *n) {
  Value* condV = n->getCondExpAST()->accept(*this); //codegen
  if (condV == 0) return 0;
  // convert condition to a bool
  condV = builder.CreateICmpNE(condV, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "ifcond");
  Function *F = builder.GetInsertBlock()->getParent();

  // automatically insert ThenBB to the end of the function F
  BasicBlock *ThenBB = BasicBlock::Create(getGlobalContext(), "then", F);
  BasicBlock *ElseBB = BasicBlock::Create(getGlobalContext(), "else");
  BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");

  // this is generated inside the cond block
  builder.CreateCondBr(condV, ThenBB, ElseBB);

  // set current insertion point to the end of ThenBB but ThenBB is empty so we are ok
  builder.SetInsertPoint(ThenBB);
  Value* thenV = n->getThenExpAST()->accept(*this); //codegen
  if (thenV == 0) return 0;
  // unconditional branch then->merge. LLVM requires that every BB has a terminator (ret or br)
  builder.CreateBr(MergeBB);
  // codegen of 'then' may change the current block i.e., it recurses to somewhere else
  // we have to pdate thenBB for PHI
  ThenBB = builder.GetInsertBlock();

  // emit else block
  F->getBasicBlockList().push_back(ElseBB); // then is added through the Create()
  builder.SetInsertPoint(ElseBB);
  Value* elseV;
  if (n->getElseExpAST() != nullptr)
    elseV = n->getElseExpAST()->accept(*this);
  else
    elseV = nullptr; // no codegen, empty block
  builder.CreateBr(MergeBB);
  // codegen of 'else' may change the current block again
  ElseBB = builder.GetInsertBlock();

  F->getBasicBlockList().push_back(MergeBB);
  builder.SetInsertPoint(MergeBB);

  // TODO (tharitt): check the type incoming. Should it always be int ?
  PHINode *PN = builder.CreatePHI(Type::getInt32Ty(getGlobalContext()), 2, "iftmp");
  PN->addIncoming(thenV, ThenBB);
  PN->addIncoming(elseV, ElseBB);
  return PN;
}

Value *LLVMGenerator::visit(IntExpAST *n) {
  return ConstantInt::get(Type::getInt32Ty(getGlobalContext()), n->getNum(), /*isSigned=*/true);
}

Value *LLVMGenerator::visit(LetExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(ProgramAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(ProjExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(SeqExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(TupleExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(UnExpAST *n) {
  return NULL;
}

Value *LLVMGenerator::visit(WhileExpAST *n) {

  Function *F = builder.GetInsertBlock()->getParent();
  BasicBlock* CondBB = BasicBlock::Create(getGlobalContext(), "whileCond", F);
  BasicBlock* LoopBB = BasicBlock::Create(getGlobalContext(), "loop");
  BasicBlock* AfterLoopBB = BasicBlock::Create(getGlobalContext(), "afterLoop");

  // connect the (whatever it is) current block to the CondBB unconditionally
  builder.CreateBr(CondBB);

  builder.SetInsertPoint(CondBB);
  Value* condV = n->getCondExpAST()->accept(*this);
  if (condV == 0) return 0;
  condV = builder.CreateICmpNE(condV, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "whileCond");
  builder.CreateCondBr(condV, /*true*/LoopBB, /*false*/AfterLoopBB); // emit right after cmp

  builder.SetInsertPoint(LoopBB);
  Value* bodyV = n->getBodyExpAST()->accept(*this); //codegen
  if (bodyV == 0) return 0;
  builder.CreateBr(CondBB); //backedge to check the condition
  // while should return unit type
  return Constant::getNullValue(Type::getInt32Ty(getGlobalContext()));
}

Type* codegenType(MyType *t){
  if (auto intTy = MyType::toIntType(t)){
    return Type::getInt32Ty(getGlobalContext());
  } else if (auto refTy = MyType::toRefType(t)){
    // ref type
  } else if (auto funTy = MyType::toFunType(t)){
    // fun type
  } else if (auto tupleTy = MyType::toTupleType(t)){
    // tutple type
    // append to vector
  }

  llvm_unreachable("Type not specified in the language");
  return NULL;
}
