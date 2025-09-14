#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class TransformPass final : public PassInfoMixin<TransformPass> {
private:
  bool runOnBasicBlock(BasicBlock &B) {
    // get the first and second instruction
    Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());

    // The address of the 1st instruction is equal to that of the 1st operand of
    // the 2nd instruction.
    assert(&Inst1st == Inst2nd.getOperand(0));

    // print the 1st instruction
    outs() << "I am the first instruction: " << Inst1st << "\n";
    // print the 1st instruction as an operand
    outs() << "Me as an operand: ";
    Inst1st.printAsOperand(outs(), false);
    outs() << "\n";

    // User-Use-Value
    outs() << "My operands:\n";
    for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
      Value *Operand = *Iter;

      if (Argument *Arg = dyn_cast<Argument>(Operand)) {
        outs() << "\tI am function " << Arg->getParent()->getName() << "\'s #"
               << Arg->getArgNo() << " argument"
               << "\n";
      }
      if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
        outs() << "\tI am a constant integer of value " << C->getValue()
               << "\n";
      }
    }

    outs() << "My users:\n";
    for (auto Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) {
      outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
    }

    outs() << "My uses (same with users):\n";
    for (auto Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) {
      outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
    }

    // Instruction Manipulation
    Instruction *NewInst = BinaryOperator::Create(
        Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));

    NewInst->insertAfter(&Inst1st);
    // Two Questions for you to answer:
    //
    // (1) Is there any alternative to updating each reference separately?
    //     Please check the documentation and try answering this.
    //
    // (2) What happens if we update the use references without the insertion?
    Inst1st.user_begin()->setOperand(0, NewInst);

    return true;
  }

  bool runOnFunction(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runOnBasicBlock(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }

public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
      if (runOnFunction(*Iter)) {
        return PreservedAnalyses::none();
      }
    }
    return PreservedAnalyses::all();
  }
};

} // anonymous namespace

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      .APIVersion = LLVM_PLUGIN_API_VERSION,
      .PluginName = "FunctionInfo",
      .PluginVersion = LLVM_VERSION_STRING,
      .RegisterPassBuilderCallbacks =
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "transform") {
                    MPM.addPass(TransformPass());
                    return true;
                  }
                  return false;
                });
          } // RegisterPassBuilderCallbacks
  };        // struct PassPluginLibraryInfo
}
