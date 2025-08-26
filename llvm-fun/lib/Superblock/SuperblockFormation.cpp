#define DEBUG_TYPE "superblock-formation"

#include "SuperblockFormation.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace fun;

/*
A  S              A      S
| /               |      |
B                 B'     B
|                 |      |
C  S2             C'     C  S2
| /               |      | /
T <--- tail       T'     T
|                  \   /
M                    M

*/

// TODO Set this number to the right value
STATISTIC(numClonedBBs, "The # of cloned basic blocks");

char SuperblockFormation::ID = 0;
static RegisterPass<SuperblockFormation>
    rp("superblock-formation", "Superblock Formation", false, false);

static cl::opt<double> biasThreshold(
    "-superblock-branch-bias-threshold", cl::init(0.7), cl::NotHidden,
    cl::Optional, cl::ValueRequired, cl::value_desc("threshold"),
    cl::desc("Branch bias threshold to trigger superblock formation"));

/* Use the given select trace algorithms and return the head BasicBlock of the
 * trace */
BasicBlock *SuperblockFormation::selectTrace(Module &m) {}

/* Highest probable BasicBlock to leave BB*/
BasicBlock *SuperblockFormation::getBestSuccessorOf(BasicBlock *BB) {}

/* Highest probable BasicBlock to enter BB */
BasicBlock *SuperblockFormation::getBestPredecessorOf(BasicBlock *BB) {}

bool SuperblockFormation::runOnModule(Module &m) {
  // You can also add other functions or variables to this class or this file

  /* Find the inner loop */

  /* Pick the trace from the inner loop */

  /* iterate over that trace until hit the BB that has side-entrace */

  /* make a 'chain' of clone after that (nothing conntect on the side) */

  /* connect this chain to the original joint */

  /* connect the end of the chain (tail') to the merge point */

  /* At the header of the side-entrance, remove the edge from trace */

  return true;
}
