#define DEBUG_TYPE "superblock-formation"

#include "SuperblockFormation.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CommandLine.h"
#include <unordered_set>

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

/* Highest probable BasicBlock to leave BB*/
BasicBlock *SuperblockFormation::getBestSuccessorOf(BasicBlock *BB) {}

/* Highest probable BasicBlock to enter BB */
BasicBlock *SuperblockFormation::getBestPredecessorOf(BasicBlock *BB) {}

/* Use the given select trace algorithms and return the head BasicBlock of the
 * trace */
template <typename T>
std::vector<std::deque<BasicBlock *>>
SuperblockFormation::selectTraceInnerMostLoop(
    Module &m, PriorityQueue<BasicBlock *, std::vector<BasicBlock *>, T> &PQ) {

  int i = 0;
  std::unordered_set<BasicBlock *> Visited;
  std::vector<std::deque<BasicBlock *>> Traces;

  // there are unvisited node
  while (!PQ.empty()) {
    // A node at the top may already get visited
    BasicBlock *Seed = PQ.top();
    PQ.pop();
    // instead of a while loop, we let this code re-enter the loop
    // and thus re-check if the PQ is empty. Otherwise, there will be
    // another check for empty (you can pop forever)
    if (Visited.find(Seed) != Visited.end())
      continue;

    Traces.push_back({});
    Traces[i].push_back(Seed);
    Visited.insert(Seed);

    BasicBlock *Current = Seed;
    // Grow trace forward
    while (true) {
      BasicBlock *Next = getBestSuccessorOf(Current);
      if (!Next)
        break;
      Traces[i].push_back(Next);
      Visited.insert(Next);
      Current = Next;
    }
    Current = Seed;
    // Grow trace backward
    while (true) {
      BasicBlock *Prev = getBestPredecessorOf(Current);
      if (!Prev)
        break;
      Traces[i].push_back(Prev);
      Visited.insert(Prev);
      Current = Prev;
    }
    ++i;
  }
  return Traces;
}

bool SuperblockFormation::runOnModule(Module &m) {
  // You can also add other functions or variables to this class or this file
  BBProfileLoader &BBProfiler = getAnalysis<BBProfileLoader>();

  auto ComparatorBB = [&](const BasicBlock *BB1, const BasicBlock *BB2) {
    // pq: the top of the heap always eval to false compared to others.
    // we want max-heap and so less is the right semantic
    return BBProfiler.BBNameToCount[BB1->getName()] <
           BBProfiler.BBNameToCount[BB2->getName()];
  };

  PriorityQueue<BasicBlock *, std::vector<BasicBlock *>, decltype(ComparatorBB)>
      PQ(ComparatorBB);

  for (auto &F : m) {
    for (auto &BB : F) {
      PQ.push(&BB);
    }
  }

  auto Traces = selectTraceInnerMostLoop(m, PQ);
  /* Find the inner loop */

  /* Pick the trace from the inner loop */

  /* iterate over that trace until hit the BB that has side-entrace */

  /* make a 'chain' of clone after that (nothing conntect on the side) */

  /* connect this chain to the original joint */

  /* connect the end of the chain (tail') to the merge point */

  /* At the header of the side-entrance, remove the edge from trace */

  return true;
}
