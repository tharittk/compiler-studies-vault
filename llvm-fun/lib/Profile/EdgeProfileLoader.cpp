#define DEBUG_TYPE "cfg-profiler"

#include "Profile/EdgeProfileLoader.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <iostream>
#include <llvm/ADT/Hashing.h>
#include <sstream>

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numEdges, "The # of edge counters loaded.");

char EdgeProfileLoader::ID = 0;
static RegisterPass<EdgeProfileLoader> rp("edge-profile-loader",
                                          "Edge Profile Loader", false, true);

static cl::opt<std::string> profileFileName(
    "edge-info-input-file", cl::init("edge_info.prof"), cl::NotHidden,
    cl::Optional, cl::ValueRequired, cl::value_desc("filename"),
    cl::desc("Edge profile info file loaded by -edge-profile-loader"));

uint64_t hashEdge(const BasicBlock &src, const BasicBlock &dest) {
  hash_code HC = hash_value(src.getParent()->getName());
  for (const auto &I : src) {
    HC = hash_combine(HC, hash_value(I.getOpcode()));
  }
  for (const auto &I : dest) {
    HC = hash_combine(HC, hash_value(I.getOpcode()));
  }
  return static_cast<uint64_t>(HC);
}

bool EdgeProfileLoader::runOnModule(Module &m) {

  std::ifstream InFile(profileFileName);
  if (!InFile.is_open()) {
    std::cerr << "Error opening file" << std::endl;
    return 1;
  }

  std::string line;
  while (std::getline(InFile, line)) {
    std::istringstream iss(line);
    std::string part;
    std::vector<std::string> parts;

    while (std::getline(iss, part, ':')) {
      parts.push_back(part);
    }
    std::string EdgeName = parts[0];
    unsigned count = std::stoi(parts[1]);
    EdgeNameToCount[EdgeName] = count;
  }
  InFile.close();
  numEdges = EdgeNameToCount["ALL"];

  return false;
}

unsigned EdgeProfileLoader::getCount(const Edge &edge) const {
  std::string EdgeName =
      (*edge.first).getName().str() + (*edge.second).getName().str();
  return EdgeNameToCount.lookup(EdgeName);
}

double EdgeProfileLoader::getWeight(const Edge &edge) const {
  const BasicBlock *src = edge.first;
  std::string EdgeName = src->getName().str() + (*edge.second).getName().str();
  auto *Terminator = src->getTerminator();
  unsigned Sum = 0;
  for (unsigned i = 0, e = Terminator->getNumSuccessors(); i != e; ++i) {
    std::string EName =
        src->getName().str() + Terminator->getSuccessor(i)->getName().str();
    Sum += static_cast<double>(EdgeNameToCount.lookup(EName));
  }
  return static_cast<double>(EdgeNameToCount.lookup(EdgeName)) / Sum;
}