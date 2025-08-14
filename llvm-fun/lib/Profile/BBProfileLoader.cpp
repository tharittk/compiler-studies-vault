#define DEBUG_TYPE "cfg-profiler"

#include "Profile/BBProfileLoader.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numBBs, "The # of BB counters loaded.");

char BBProfileLoader::ID = 0;
static RegisterPass<BBProfileLoader>
rp("bb-profile-loader", "Basic Block Profile Loader", false, true);

static cl::opt<std::string>
profileFileName("bb-info-input-file", cl::init("bb_info.prof"),
                cl::NotHidden, cl::Optional, cl::ValueRequired,
                cl::value_desc("filename"),
                cl::desc("BB profile info file loaded by -bb-profile-loader"));

bool BBProfileLoader::runOnModule(Module &m) {
  // TODO Implement your logic here

  return false;
}
