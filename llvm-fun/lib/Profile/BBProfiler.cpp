#define DEBUG_TYPE "cfg-profiler"

#include "BBProfiler.h"
#include "ProfileUtils.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace fun;

// TODO Set this number to the right value
STATISTIC(numBBs, "The # of BBs profiled.");

char BBProfiler::ID = 0;
static RegisterPass<BBProfiler> rp("bb-profiler", "Basic Block Profiler");

static cl::opt<std::string>
outputFileName("bb-info-output-file", cl::init("bb_info.prof"),
    cl::NotHidden, cl::Optional, cl::ValueRequired,
    cl::value_desc("filename"),
    cl::desc("Output profile info file (used by -bb-profiler)"));

bool BBProfiler::runOnModule(Module &m) {
  // TODO Implement your logic here

  return true;
}
