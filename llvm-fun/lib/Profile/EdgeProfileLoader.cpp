#define DEBUG_TYPE "cfg-profiler"

#include "Profile/EdgeProfileLoader.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numEdges, "The # of edge counters loaded.");

char EdgeProfileLoader::ID = 0;
static RegisterPass<EdgeProfileLoader>
rp("edge-profile-loader", "Edge Profile Loader", false, true);

static cl::opt<std::string>
profileFileName("edge-info-input-file", cl::init("edge_info.prof"),
             cl::NotHidden, cl::Optional, cl::ValueRequired,
             cl::value_desc("filename"),
             cl::desc("Edge profile info file loaded by -edge-profile-loader"));

bool EdgeProfileLoader::runOnModule(Module &m) {
  // TODO Implement your logic here

  return false;
}
