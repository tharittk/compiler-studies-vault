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

STATISTIC(numClonedBBs, "The # of cloned basic blocks");

char SuperblockFormation::ID = 0;
static RegisterPass<SuperblockFormation>
    rp("superblock-formation", "Superblock Formation", false, false);

static cl::opt<double> biasThreshold(
    "-superblock-branch-bias-threshold", cl::init(0.7), cl::NotHidden,
    cl::Optional, cl::ValueRequired, cl::value_desc("threshold"),
    cl::desc("Branch bias threshold to trigger superblock formation"));

/* Highest probable BasicBlock to leave BB*/
BasicBlock *SuperblockFormation::getBestSuccessorOf(
    BasicBlock *BB, Loop *InnerLoop,
    std::unordered_set<BasicBlock *> &Visited) {
  // get edge with the hightest probability leaving BB
  EdgeProfileLoader &EdgeProfiler = getAnalysis<EdgeProfileLoader>();
  LoopInfo &LI = getAnalysis<LoopInfo>();

  double MaxWeight = 0.0;
  BasicBlock *Dest;

  for (succ_iterator SI = succ_begin(BB); SI != succ_end(BB); ++SI) {
    BasicBlock *Succ = *SI;
    double Weight = EdgeProfiler.getWeight(EdgeProfiler.getEdge(BB, Succ));
    if (Weight > MaxWeight) {
      MaxWeight = Weight;
      Dest = Succ;
    }
  }

  // check if this is a backedge
  if (InnerLoop->getHeader() == Dest)
    return nullptr;

  if (MaxWeight < biasThreshold)
    return nullptr;
  // Check if dest is outside the loop
  Loop *ContainingLoop = LI.getLoopFor(Dest);
  if (!ContainingLoop || (ContainingLoop != InnerLoop))
    return nullptr;

  return Dest;
}

/* Highest probable BasicBlock to enter BB */
BasicBlock *SuperblockFormation::getBestPredecessorOf(
    BasicBlock *BB, Loop *InnerLoop,
    std::unordered_set<BasicBlock *> &Visited) {

  if (InnerLoop->getHeader() == BB)
    return nullptr;

  // get edge with the hightest probability entering BB
  EdgeProfileLoader &EdgeProfiler = getAnalysis<EdgeProfileLoader>();
  LoopInfo &LI = getAnalysis<LoopInfo>();

  double MaxWeight = 0.0;
  BasicBlock *Src;

  for (pred_iterator PI = pred_begin(BB); PI != pred_end(BB); ++PI) {
    BasicBlock *Pred = *PI;
    double Weight = EdgeProfiler.getWeight(EdgeProfiler.getEdge(Pred, BB));
    if (Weight > MaxWeight) {
      MaxWeight = Weight;
      Src = Pred;
    }
  }

  if (MaxWeight < biasThreshold)
    return nullptr;

  // Src is already visited
  if (Visited.find(Src) != Visited.end())
    return nullptr;

  // Check if dest is outside the loop
  Loop *ContainingLoop = LI.getLoopFor(Src);
  if (!ContainingLoop || (ContainingLoop != InnerLoop))
    return nullptr;

  return Src;
}

/* Use the given select trace algorithms and return the head BasicBlock of the
 * trace */
template <typename T>
std::vector<std::deque<BasicBlock *>>
SuperblockFormation::selectTraceInnerMostLoop(
    Module &m, Loop *InnerLoop,
    PriorityQueue<BasicBlock *, std::vector<BasicBlock *>, T> &PQ) {

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
      BasicBlock *Next = getBestSuccessorOf(Current, InnerLoop, Visited);
      if (!Next)
        break;
      Traces[i].push_back(Next);
      Visited.insert(Next);
      Current = Next;
    }
    Current = Seed;
    // Grow trace backward
    while (true) {
      BasicBlock *Prev = getBestPredecessorOf(Current, InnerLoop, Visited);
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

void findInnermostLoops(Loop *L, std::vector<Loop *> &InnerLoops) {

  std::vector<Loop *> Inners = L->getSubLoops();
  if (Inners.empty()) {
    InnerLoops.push_back(L);
  } else {
    // Recurse
    for (Loop *SubLoop : Inners)
      findInnermostLoops(SubLoop, InnerLoops);
  }
}

int getPredCount(BasicBlock *BB) {
  int count = 0;
  for (pred_iterator PI = pred_begin(BB); PI != pred_end(BB); ++PI) {
    ++count;
  }
  return count;
}

void changeSuccessor(BasicBlock *Src, BasicBlock *OldDest,
                     BasicBlock *NewDest) {

  Instruction *TermInst = Src->getTerminator();
  if (BranchInst *BrInst = dyn_cast<BranchInst>(TermInst); BrInst != nullptr) {
    if (BrInst->isUnconditional()) {
      BrInst->setSuccessor(0, NewDest);
    } else {
      if (BrInst->getSuccessor(0) == OldDest) {
        BrInst->setSuccessor(0, NewDest);
      } else if (BrInst->getSuccessor(1) == OldDest) {
        BrInst->setSuccessor(1, NewDest);
      } else {
        llvm_unreachable("change successor: old successor does not match");
      }
    }
  }
}

bool SuperblockFormation::runOnModule(Module &m) {
  // You can also add other functions or variables to this class or this file
  BBProfileLoader &BBProfiler = getAnalysis<BBProfileLoader>();
  LoopInfo &LI = getAnalysis<LoopInfo>();

  auto ComparatorBB = [&](const BasicBlock *BB1, const BasicBlock *BB2) {
    // pq: the top of the heap always eval to false compared to others.
    // we want max-heap and so less is the right semantic
    return BBProfiler.BBNameToCount[BB1->getName()] <
           BBProfiler.BBNameToCount[BB2->getName()];
  };

  /* Find the inner loop recursively */
  std::vector<Loop *> InnerLoops;
  for (LoopInfo::iterator LIt = LI.begin(); LIt != LI.end(); ++LIt) {
    Loop *L = *LIt;
    findInnermostLoops(L, InnerLoops);
  }

  // Code motion for each innner loop
  for (Loop *L : InnerLoops) {

    PriorityQueue<BasicBlock *, std::vector<BasicBlock *>,
                  decltype(ComparatorBB)>
        PQ(ComparatorBB);

    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end();
         ++BI) {
      PQ.push(*BI);
    }

    /* Pick the traces from the inner loop - first trace in a vector Traces is
     * the hottest*/
    auto Traces = selectTraceInnerMostLoop(m, L, PQ);
    /* Tail duplication */

    /* iterate over that trace until hit the BB that has side-entrace */
    std::deque<BasicBlock *> HotTrace = Traces[0];

    bool beginClone = false;

    ValueToValueMapTy VMap; // A map to track cloned values
    const Twine NameSuffix = ".cloned";
    BasicBlock *PrevBB, *ClonedBB;
    for (auto it = HotTrace.begin(); it != HotTrace.end(); ++it) {

      BasicBlock *CurrBB = *it;
      /* side entrace first detected, must cloen til the end of trace*/
      if (!beginClone && getPredCount(CurrBB) > 1) {
        beginClone = true;
      }
      if (beginClone) {
        ClonedBB =
            CloneBasicBlock(CurrBB, VMap, NameSuffix, CurrBB->getParent());
        changeSuccessor(PrevBB, CurrBB, ClonedBB);
      }
      PrevBB = CurrBB;
    }

    /* connect the end of the chain (tail') to the merge point */
    BranchInst::Create(PrevBB->getTerminator()->getSuccessor(0), ClonedBB);
  }

  return true;
}
