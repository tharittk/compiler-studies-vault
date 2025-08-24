#define DEBUG_TYPE "superblock-formation"

#include "SuperblockFormation.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numClonedBBs, "The # of cloned basic blocks");

char SuperblockFormation::ID = 0;
static RegisterPass<SuperblockFormation>
    rp("superblock-formation", "Superblock Formation", false, false);

static cl::opt<double> biasThreshold(
    "-superblock-branch-bias-threshold", cl::init(0.7), cl::NotHidden,
    cl::Optional, cl::ValueRequired, cl::value_desc("threshold"),
    cl::desc("Branch bias threshold to trigger superblock formation"));

bool SuperblockFormation::runOnModule(Module &m) {
  // TODO Implement your logic here
  // You can also add other functions or variables to this class or this file
  return true;
}
