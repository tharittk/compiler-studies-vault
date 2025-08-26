#define DEBUG_TYPE "cfg-profiler"

#include "ProfilePrinter.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>
using namespace llvm;
using namespace fun;

char ProfilePrinter::ID = 0;
static RegisterPass<ProfilePrinter>
    rp("profile-printer", "Profile Information Printer", false, true);

static cl::opt<std::string>
    dumpFile("profile-printer-dump-file", cl::init("prof_dump.txt"),
             cl::NotHidden, cl::Optional, cl::ValueRequired,
             cl::value_desc("filename"),
             cl::desc("Proifle info dump file used by -profile-printer"));

static cl::opt<bool> toDumpBBInfo("dump-bb-info", cl::init(false),
                                  cl::NotHidden, cl::Optional,
                                  cl::desc("Print BB profiling info"));

static cl::opt<bool> toDumpEdgeInfo("dump-edge-info", cl::init(false),
                                    cl::NotHidden, cl::Optional,
                                    cl::desc("Print edge profiling info"));

void ProfilePrinter::getAnalysisUsage(AnalysisUsage &au) const {
  if (toDumpBBInfo)
    au.addRequired<BBProfileLoader>();
  if (toDumpEdgeInfo)
    au.addRequired<EdgeProfileLoader>();
  au.setPreservesAll();
}

bool ProfilePrinter::runOnModule(Module &m) {
  std::ofstream fout(dumpFile, std::ios::out | std::ios::trunc);
  if (!fout.is_open()) {
    errs() << "File open error: " << dumpFile << "\n";
    exit(1);
  }

  if (toDumpBBInfo)
    dumpBBInfo(m, fout);
  if (toDumpEdgeInfo)
    dumpEdgeInfo(m, fout);

  fout.flush();
  return false;
}

void ProfilePrinter::dumpBBInfo(const Module &m, std::ofstream &fout) {
  BBProfileLoader &loader = getAnalysis<BBProfileLoader>();
  fout << "-- Basic block information:\n\n";

  for (Module::const_iterator f = m.begin(), e = m.end(); f != e; ++f) {
    if (f->isDeclaration())
      continue;

    // Print BBs in alphabetical order
    fout << "Function: " << f->getName().str() << "\n";

    std::map<std::string, const BasicBlock *> name2BBMap;
    for (Function::const_iterator bb = f->begin(), e = f->end(); bb != e; ++bb)
      name2BBMap[bb->getName()] = bb;

    for (auto &kv : name2BBMap)
      fout << kv.first << ": " << loader.getCount(kv.second) << "\n";
    fout << "\n";
  }
  fout << "\n";
}

void ProfilePrinter::dumpEdgeInfo(const Module &m, std::ofstream &fout) {
  EdgeProfileLoader &loader = getAnalysis<EdgeProfileLoader>();
  fout << "-- Edge information:\n\n";

  for (Module::const_iterator f = m.begin(), e = m.end(); f != e; ++f) {
    if (f->isDeclaration())
      continue;

    // Print BBs / predecessors in alphabetical order
    fout << "Function: " << f->getName().str() << "\n";

    std::map<std::string, const BasicBlock *> name2BBMap;
    for (Function::const_iterator bb = f->begin(), e = f->end(); bb != e; ++bb)
      name2BBMap[bb->getName()] = bb;

    for (auto &kv : name2BBMap) {
      const BasicBlock *bb = kv.second;
      const TerminatorInst *ti = bb->getTerminator();

      std::map<std::string, const BasicBlock *> name2PredMap;
      for (unsigned s = 0, e = ti->getNumSuccessors(); s != e; ++s) {
        BasicBlock *succ = ti->getSuccessor(s);
        name2PredMap[succ->getName()] = succ;
      }

      for (auto &kv : name2PredMap) {
        const BasicBlock *succ = kv.second;
        EdgeProfileLoader::Edge edge = EdgeProfileLoader::getEdge(bb, succ);
        std::cout << "Get name succ " << succ->getName().str() << "\n";
        fout << bb->getName().str() << " -> " << succ->getName().str() << ": "
             << loader.getCount(edge) << " (" << loader.getWeight(edge) * 100
             << "%)\n";
      }
    }
    fout << "\n";
  }
  fout << "\n";
}
