#define DEBUG_TYPE "cfg-profiler"

#include "Profile/BBProfileLoader.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <iostream>
#include <llvm/ADT/Hashing.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <sstream>
#include <vector>

using namespace llvm;
using namespace fun;

STATISTIC(numBBs, "The # of BB counters loaded.");

char BBProfileLoader::ID = 0;
static RegisterPass<BBProfileLoader>
    rp("bb-profile-loader", "Basic Block Profile Loader", false, true);

static cl::opt<std::string> profileFileName(
    "bb-info-input-file", cl::init("bb_info.prof"), cl::NotHidden, cl::Optional,
    cl::ValueRequired, cl::value_desc("filename"),
    cl::desc("BB profile info file loaded by -bb-profile-loader"));

uint64_t hashBB(const BasicBlock &BB) {
  using namespace llvm;
  llvm::hash_code HC = hash_value(BB.getParent()->getName());
  for (const auto &I : BB) {
    HC = hash_combine(HC, hash_value(I.getOpcode()));
  }
  return static_cast<uint64_t>(HC);
}

bool BBProfileLoader::runOnModule(Module &m) {
  // STEP 1: Hash BasicBlock* to name
  for (auto &F : m) {
    int BBNum = 0;
    for (const auto &BB : F) {
      std::string BBName = F.getName().str() + "_BB_" + std::to_string(BBNum);
      BBToBBName.insert(std::pair<uint64_t, std::string>({hashBB(BB), BBName}));
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
    std::string BBName = parts[0];
    unsigned count = std::stoi(parts[1]);
    BBNameToCount[BBName] = count;
  }
  InFile.close();
  numBBs = BBNameToCount["TotalBlocks"];

  return false;
}

unsigned BBProfileLoader::getCount(const BasicBlock *bb) const {
  std::string BBName = BBToBBName.lookup(hashBB(*bb));
  // unsigned count = BBNameToCount.lookup(BBName);
  return BBNameToCount.lookup(BBName);
}
