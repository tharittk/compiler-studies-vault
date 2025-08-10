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
    return builder.CreateAdd(L, R, "addInt");
  case OpKind::OP_Sub:
    return builder.CreateSub(L, R, "subInt");
  case OpKind::OP_Mul:
    return builder.CreateMul(L, R, "mulInt");
  case OpKind::OP_Equal:
  {
    Value *cond = builder.CreateICmp(CmpInst::ICMP_EQ, L, R, "eqInt");
    Value *condZext = builder.CreateZExt(cond, Type::getInt32Ty(getGlobalContext()));
    return condZext;
  }
  case OpKind::OP_LT:
  {
    Value *cond = builder.CreateICmp(CmpInst::ICMP_SLT, L, R, "ltInt");
    Value *condZext = builder.CreateZExt(cond, Type::getInt32Ty(getGlobalContext()));
    return condZext;
  }
  case OpKind::OP_And:
  {
    // e1 && e2 → short-circuit
    Function *F = builder.GetInsertBlock()->getParent();

    BasicBlock *rhsBlock = BasicBlock::Create(getGlobalContext(), "and.rhs", F);
    BasicBlock *mergeBlock = BasicBlock::Create(getGlobalContext(), "and.merge", F);

    Value *condE1 = builder.CreateICmpNE(
        L, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "ande1");
    BasicBlock *lhsBlock = builder.GetInsertBlock(); // save predecessor before changing IP

    builder.CreateCondBr(condE1, rhsBlock, mergeBlock);

    // Evaluate e2 only if e1 != 0
    builder.SetInsertPoint(rhsBlock);
    Value *condE2 = builder.CreateICmpNE(
        R, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "ande2");
    builder.CreateBr(mergeBlock);

    // Merge result
    builder.SetInsertPoint(mergeBlock);
    PHINode *phi = builder.CreatePHI(Type::getInt32Ty(getGlobalContext()), 2, "and.result");
    phi->addIncoming(ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), lhsBlock);
    phi->addIncoming(builder.CreateZExt(condE2, Type::getInt32Ty(getGlobalContext())), rhsBlock);
    return phi;
  }

  case OpKind::OP_Or:
  {
    // e1 || e2 → short-circuit
    Function *F = builder.GetInsertBlock()->getParent();

    BasicBlock *rhsBlock = BasicBlock::Create(getGlobalContext(), "or.rhs", F);
    BasicBlock *mergeBlock = BasicBlock::Create(getGlobalContext(), "or.merge", F);

    Value *condE1 = builder.CreateICmpNE(
        L, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "ore1");

    BasicBlock *lhsBlock = builder.GetInsertBlock();
    builder.CreateCondBr(condE1, mergeBlock, rhsBlock);

    // Merge result: if e1 != 0 → 1, else evaluate e2
    builder.SetInsertPoint(rhsBlock);
    Value *condE2 = builder.CreateICmpNE(
        R, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "ore2");

    Value *condE2Zext = builder.CreateZExt(condE2, Type::getInt32Ty(getGlobalContext()));
    builder.CreateBr(mergeBlock);

    builder.SetInsertPoint(mergeBlock);
    PHINode *phi = builder.CreatePHI(Type::getInt32Ty(getGlobalContext()), 2, "or.result");
    phi->addIncoming(ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 1), lhsBlock);
    phi->addIncoming(condE2Zext, rhsBlock);
    return phi;
  }

  case OpKind::OP_Set:
  {
    if (auto *allocInst = dyn_cast<AllocaInst>(L))
      return builder.CreateStore(R, allocInst);
    return builder.CreateStore(R, L);
  }
  default:
  {
    reportErrorAndExit(n->getSrcLoc(), "BinExp Error: No case matches");
    return nullptr;
  }
  }
}

Value *LLVMGenerator::visit(CallExpAST *n)
{
  // simple because our language supports only one variable
  Value *funV = n->getFunExpAST()->accept(*this);
  if (!dyn_cast<Function>(funV))
    reportErrorAndExit(n->getSrcLoc(), "CallExp Error: dyn_cast test of funV must be Function Type");

  // LLVM handles the stack arithmatic
  Function *CalleeF = module->getFunction(funV->getName());
  Value *argV = n->getArgExpAST()->accept(*this);
  return builder.CreateCall(CalleeF, argV, "call");
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

  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
  builder.SetInsertPoint(BB);

  // Store the supplied function argument to the allocate slot in stack
  Value *allocaInst = builder.CreateAlloca(toLLVMType(paramTy), nullptr, AI->getName());
  builder.CreateStore(AI, allocaInst);
  ctxt.bind(n->getParamName(), allocaInst);

  // TODO: move it here? so that it allows recursive call
  // TODO: this binding is for IdExpAST parsing (ctxt->F)
  // but the actual function instance is accessed through globalContext() still
  // Is this redundant ?
  ctxt.bind(n->getName(), F);
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

  Value *v = ctxt.get(n->getName());
  if (auto *allocInst = dyn_cast<AllocaInst>(v))
    return builder.CreateLoad(allocInst, "loadId");
  return v;
}

Value *LLVMGenerator::visit(IfExpAST *n)
{
  Value *condV = n->getCondExpAST()->accept(*this); // codegen
  if (condV == 0)
    return 0;
  // convert condition to a bool
  condV = builder.CreateICmpNE(condV, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "ifcondI");
  Function *F = builder.GetInsertBlock()->getParent();

  // automatically insert ThenBB to the end of the function F
  BasicBlock *ThenBB = BasicBlock::Create(getGlobalContext(), "then", F);
  BasicBlock *ElseBB = BasicBlock::Create(getGlobalContext(), "else");
  BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");

  // this is generated inside the cond block
  builder.CreateCondBr(condV, ThenBB, ElseBB);

  // set current insertion point to the end of ThenBB but ThenBB is empty so we are ok
  builder.SetInsertPoint(ThenBB);
  Value *thenV = n->getThenExpAST()->accept(*this);
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
  Value *ptr = builder.CreateStructGEP(targetTupleAlloca, n->getIndex(), "loadPtr");
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
    Value *v = n->getExp1AST()->accept(*this);
    assert(v && "Operand for NOT is null");
    // Compare with 0 → yields i1 (true if equal to 0)
    Value *isZero = builder.CreateICmpEQ(
        v,
        ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0),
        "isZero");
    // Zero-extend i1 to i32 (0 → 0, true → 1)
    Value *result = builder.CreateZExt(isZero, Type::getInt32Ty(getGlobalContext()), "not");
    return result;
  }
  case OpKind::OP_Ref:
  {
    // Fun's semantic use heap
    Function *F = builder.GetInsertBlock()->getParent();
    Value *expV = n->getExp1AST()->accept(*this);
    Value *heapInst = allocHeap(expV->getType(), &F->getEntryBlock(), "allocRefHeap");
    return heapInst;
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
    std::vector<Type *> llvmTys;
    MyTupleType *mt = dyn_cast<MyTupleType>(t);
    for (const auto ty : mt->getTypes())
    {
      llvmTys.push_back(toLLVMType(ty));
    }
    return StructType::get(getGlobalContext(), llvmTys);
  }
  else
  {
    /* likely function type */
    std::cerr << "You use type from MyType -> LLVM Type the wrong way !";
    return Type::getVoidTy(getGlobalContext());
  }
}