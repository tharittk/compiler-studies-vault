#define DEBUG_TYPE "cfg-profiler"

#include "EdgeProfiler.h"
#include "ProfileUtils.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numEdges, "The # of edges profiled.");

char EdgeProfiler::ID = 0;
static RegisterPass<EdgeProfiler> rp("edge-profiler", "Edge Profiler");

static cl::opt<std::string>
outputFileName("edge-info-output-file", cl::init("edge_info.prof"),
    cl::NotHidden, cl::Optional, cl::ValueRequired,
    cl::value_desc("filename"),
    cl::desc("Output profile info file (used by -edge-profiler)"));

bool EdgeProfiler::runOnModule(Module &m) {
  // TODO Implement your logic here

  return true;
}
