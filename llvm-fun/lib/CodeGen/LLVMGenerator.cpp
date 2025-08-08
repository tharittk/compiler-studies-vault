#include "CodeGen/LLVMGenerator.h"
#include "AST/AST.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Verifier.h"

using namespace fun;
using namespace llvm;

void LLVMGenerator::run(std::ostream &out)
{
  InitializeNativeTarget();
  module = new Module(srcFileName, getGlobalContext());
  // Actually we don't need JIT feature in this assignment, but we create this
  // to get DataLayout and TargetMachine instances.
  std::string errStr;
  ExecutionEngine *engine = EngineBuilder(module).setErrorStr(&errStr).create();
  if (!engine)
  {
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

Value *LLVMGenerator::allocHeap(Type *t, BasicBlock *bb, std::string name)
{
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

Value *LLVMGenerator::visit(BinExpAST *n)
{
  auto leftExp = n->getExp1AST();
  auto rightExp = n->getExp2AST();

  Value *L = leftExp->accept(*this);
  Value *R = rightExp->accept(*this);
  if (!L || !R)
    reportErrorAndExit(n->getSrcLoc(), "BinExp Error: L or R Value is null");

  switch (n->getOp())
  {
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
  case OpKind::OP_Set:
    return this->builder.CreateStore(R, L);
  default:
  {
    reportErrorAndExit(n->getSrcLoc(), "BinExp Error: No case matches");
    return nullptr;
  }
  }
}

Value *LLVMGenerator::visit(CallExpAST *n)
{
  auto funExp = n->getFunExpAST();
  auto argExp = n->getArgExpAST();

  // simple because our language supports only one variable
  Value *funV = funExp->accept(*this);
  if (!dyn_cast<Function>(funV))
    reportErrorAndExit(n->getSrcLoc(), "CallExp Error: dyn_cast test of funV must be Function Type");
  Function *CalleeF = module->getFunction(funV->getName());

  Value *argV = argExp->accept(*this);
  return builder.CreateCall(CalleeF, argV, "callf");
}

Value *LLVMGenerator::visit(ConstrainExpAST *n)
{
  // constrain is for type checking. It doesn't make
  // any difference in IR generation
  return n->getExpAST()->accept(*this);
}

Value *LLVMGenerator::visit(FunDeclAST *n)
{
  // register function to environment
  auto retTy = MyType::getType(n->getRetTypeAST());
  auto paramTy = MyType::getType(n->getParamTypeAST());
  // case swtich, int, ref, fun ,tuple, unit
  FunctionType *FT = FunctionType::get(/*return ty*/ toLLVMType(retTy),
                                       /*arg ty*/ toLLVMType(paramTy), /*vararg*/ false);

  // register to the module for symbol look up
  Function *F = Function::Create(FT, Function::ExternalLinkage, n->getName(), module);

  // llvm semantic, if the symbol already exists, F will be automatically renamed
  // and thus not equal to intended name
  if (F->getName() != n->getName())
  {
    F->eraseFromParent();
    F = module->getFunction(n->getName());
  }

  // function in our language takes only one argument
  Function::arg_iterator AI = F->arg_begin();
  AI->setName(n->getParamName());

  // no need for this since LLVM uses module "context" for functions
  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
  builder.SetInsertPoint(BB);
  if (Value *retVal = n->getBodyExpAST()->accept(*this))
  {
    builder.CreateRet(retVal);
    verifyFunction(*F);
    return F;
  }
  return NULL;
}

Value *LLVMGenerator::visit(IdExpAST *n)
{
  if (!ctxt.has(n->getName()))
  {
    auto msg = "IdExp Error: " + n->getName() + " is not in env";
    reportErrorAndExit(n->getSrcLoc(), msg);
  }
  return ctxt.get(n->getName());
}

Value *LLVMGenerator::visit(IfExpAST *n)
{
  Value *condV = n->getCondExpAST()->accept(*this); // codegen
  if (condV == 0)
    return 0;
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
  Value *thenV = n->getThenExpAST()->accept(*this); // codegen
  if (thenV == 0)
    return 0;
  // unconditional branch then->merge. LLVM requires that every BB has a terminator (ret or br)
  builder.CreateBr(MergeBB);
  // codegen of 'then' may change the current block i.e., it recurses to somewhere else
  // we have to pdate thenBB for PHI
  ThenBB = builder.GetInsertBlock();

  // emit else block
  F->getBasicBlockList().push_back(ElseBB); // then is added through the Create()
  builder.SetInsertPoint(ElseBB);
  Value *elseV;
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

Value *LLVMGenerator::visit(IntExpAST *n)
{
  return ConstantInt::get(Type::getInt32Ty(getGlobalContext()), n->getNum(), /*isSigned=*/true);
}

Value *LLVMGenerator::visit(LetExpAST *n)
{
  auto varName = n->getVarName();
  Value *varExpV = n->getVarExpAST()->accept(*this);
  auto ck = ctxt.checkPoint();
  ctxt.bind(varName, varExpV);
  // eval bodyVal from codegen
  Value *bodyV = n->getBodyExpAST()->accept(*this);
  // restore old variable
  ctxt.restoreCheckPoint(ck);
  return bodyV;
}

Value *LLVMGenerator::visit(ProgramAST *n)
{
  const std::map<std::string, FunDeclAST *> &funDecls = n->getFunDeclASTs();
  for (auto it = funDecls.begin(); it != funDecls.end(); ++it)
  {
    it->second->accept(*this);
  }
  return NULL;
}

Value *LLVMGenerator::visit(ProjExpAST *n)
{
  Value *targetTupleAlloca = n->getTargetTupleExpAST()->accept(*this);
  // if (!tupleV) // must be tuple/vector, depending on how you represent it
  //   reportErrorAndExit(n->getSrcLoc(), "ProjExp Error: target Value must be a vector");
  Value *ptr = builder.CreateStructGEP(targetTupleAlloca, n->getIndex(), "load.ptr");
  Value *loadInst = builder.CreateLoad(ptr, "projLoad");
  return loadInst;
}

Value *LLVMGenerator::visit(SeqExpAST *n)
{
  n->getExp1AST()->accept(*this);
  Value *v = n->getExp2AST()->accept(*this);
  return v;
}

Value *LLVMGenerator::visit(TupleExpAST *n)
{
  std::vector<Value *> tupleV;
  std::vector<Type *> tupleTy;

  for (const auto exp : n->getExpASTs())
  {
    Value *expV = exp->accept(*this);
    tupleV.push_back(expV);
    tupleTy.push_back(expV->getType());
  }

  StructType *elemTy = StructType::get(getGlobalContext(), tupleTy);
  Value *structAlloca = builder.CreateAlloca(elemTy, nullptr, "tuple.alloca");

  for (size_t i = 0; i < tupleV.size(); ++i)
  {
    Value *fieldPtr = builder.CreateStructGEP(structAlloca, i, "tuple.ptr");
    builder.CreateStore(tupleV[i], fieldPtr);
  }
  return structAlloca;
}

Value *LLVMGenerator::visit(UnExpAST *n)
{

  switch (n->getOp())
  {
  case OpKind::OP_UMinus:
  {
    // apply minus to exp
    Value *v = n->getExp1AST()->accept(*this);
    ConstantInt *constInt = dyn_cast<ConstantInt>(v);
    if (!constInt)
      reportErrorAndExit(n->getSrcLoc(), "UMinus must be applied to Int");
    return ConstantInt::get(v->getType(), -constInt->getValue());
  }
  case OpKind::OP_Not:
  {
    // compare with zero,
    Function *F = builder.GetInsertBlock()->getParent();
    BasicBlock *condBB = BasicBlock::Create(getGlobalContext(), "notCond", F);
    BasicBlock *trueBB = BasicBlock::Create(getGlobalContext(), "trueBr");
    BasicBlock *falseBB = BasicBlock::Create(getGlobalContext(), "falseBr");
    BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "mergeNot");

    builder.CreateBr(condBB);
    builder.SetInsertPoint(condBB);

    Value *v = n->getExp1AST()->accept(*this);
    Value *condV = builder.CreateICmpNE(v, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "notCond");
    builder.CreateCondBr(condV, /*true*/ trueBB, /*false*/ falseBB);

    F->getBasicBlockList().push_back(trueBB);
    builder.SetInsertPoint(trueBB);
    Value *zeroV = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0, /*isSigned=*/true);

    builder.CreateBr(MergeBB);

    F->getBasicBlockList().push_back(falseBB);
    builder.SetInsertPoint(falseBB);
    Value *oneV = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 1, /*isSigned=*/true);
    builder.CreateBr(MergeBB);

    F->getBasicBlockList().push_back(MergeBB);
    builder.SetInsertPoint(MergeBB);

    PHINode *PN = builder.CreatePHI(Type::getInt32Ty(getGlobalContext()), 2, "nottmp");
    PN->addIncoming(zeroV, trueBB);
    PN->addIncoming(oneV, falseBB);
    return PN;
  }
  case OpKind::OP_Ref:
  {
    // create a ptr from it
    Value *expV = n->getExp1AST()->accept(*this);
    Value *allocInst = builder.CreateAlloca(expV->getType(), expV, "allocaRef");
    Value *ptr = builder.CreateGEP(allocInst, 0, "refPtr");
    return ptr;
  }

  case OpKind::OP_Get:
  {
    Value *expV = n->getExp1AST()->accept(*this);
    // you may do dyn_cast here
    return builder.CreateLoad(expV, "loadGet");
  }
  default:
    reportErrorAndExit(n->getSrcLoc(), "UnExp Error: case does not match");
    return NULL;
  }
}

Value *LLVMGenerator::visit(WhileExpAST *n)
{

  Function *F = builder.GetInsertBlock()->getParent();
  BasicBlock *CondBB = BasicBlock::Create(getGlobalContext(), "whileCond", F);
  BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop");
  BasicBlock *AfterLoopBB = BasicBlock::Create(getGlobalContext(), "afterLoop");

  // connect the (whatever it is) current block to the CondBB unconditionally
  builder.CreateBr(CondBB);

  builder.SetInsertPoint(CondBB);
  Value *condV = n->getCondExpAST()->accept(*this);
  if (condV == 0)
    return 0;
  condV = builder.CreateICmpNE(condV, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "whileCond");
  builder.CreateCondBr(condV, /*true*/ LoopBB, /*false*/ AfterLoopBB); // emit right after cmp

  builder.SetInsertPoint(LoopBB);
  Value *bodyV = n->getBodyExpAST()->accept(*this); // codegen
  if (bodyV == 0)
    return 0;
  builder.CreateBr(CondBB); // backedge to check the condition
  // while should return unit type
  return Constant::getNullValue(Type::getInt32Ty(getGlobalContext()));
}

Type *LLVMGenerator::toLLVMType(MyType *t)
{
  if (t->isIntType())
  {
    return Type::getInt32Ty(getGlobalContext());
  }
  else if (t->isRefType())
  {
    return Type::getInt32PtrTy(getGlobalContext(), true);
  }
  else if (t->isTupleType())
  {
    // TODO: what should be here ? The is no generalize struct type
    return Type::getVoidTy(getGlobalContext());
  }
  else
  {
    /* likely function type */
    std::cerr << "You use type from MyType -> LLVM Type the wrong way !";
    return Type::getVoidTy(getGlobalContext());
  }
}