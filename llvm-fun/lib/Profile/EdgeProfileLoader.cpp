#define DEBUG_TYPE "cfg-profiler"

#include "Profile/EdgeProfileLoader.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/CFG.h"
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

  // STEP 1: Build Hash table for input from table
  for (auto &F : m) {
    unsigned BBNum = 0;
    for (auto &BB : F) {
      unsigned EdgeIndex = 0;
      for (auto succ = succ_begin(&BB); succ != succ_end(&BB); ++succ) {
        std::string EdgeName = F.getName().str() + std::string("_BB_") +
                               std::to_string(BBNum) + std::string("_E_") +
                               std::to_string(EdgeIndex);

        EdgeToEdgeName.insert(std::pair<uint64_t, std::string>(
            {hashEdge(BB, *(*succ)), EdgeName}));
        ++EdgeIndex;
      }
      ++BBNum;
    }
  }
  // STEP 2: Read file and map BB Name -> counts
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
    if (parts.size() == 2) {
      unsigned count = std::stoi(parts[1]);
      EdgeNameToCount[EdgeName] = count;
    }
  }
  InFile.close();
  numEdges = EdgeNameToCount["TotalEdges"];

  return false;
}

unsigned EdgeProfileLoader::getCount(const Edge &edge) const {
  std::string EdgeName =
      EdgeToEdgeName.lookup(hashEdge(*edge.first, *edge.second));

  return EdgeNameToCount.lookup(EdgeName);
}

double EdgeProfileLoader::getWeight(const Edge &edge) const {

  std::string EdgeName =
      EdgeToEdgeName.lookup(hashEdge(*edge.first, *edge.second));

  size_t pos = EdgeName.find("_E_");
  std::string PrefixEdgeName = EdgeName.substr(0, pos + 3);
  // get the count of over block that leaves this
  unsigned Sum = 0;
  const BasicBlock *src = edge.first;
  unsigned EdgeIndex = 0;
  for (auto succ = succ_begin(src); succ != succ_end(src); ++succ) {
    std::string Name = PrefixEdgeName + std::to_string(EdgeIndex);
    Sum += static_cast<double>(EdgeNameToCount.lookup(Name));
    ++EdgeIndex;
  }
  return static_cast<double>(EdgeNameToCount.lookup(EdgeName)) / Sum;
}