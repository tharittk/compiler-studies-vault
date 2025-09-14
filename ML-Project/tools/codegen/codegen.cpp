#include "CodeGen/LLVMGenerator.h"
#include "AST/AST.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <getopt.h>

using namespace fun;

extern ProgramAST *program;

int yylex();
int yyparse();
extern FILE *yyin;

void printUsageAndExit(std::string prgName) {
  std::cerr << "Usage: " << prgName << " [options] FUN_FILE\n";
  std::cerr << "Options:\n";
  std::cerr << "  -o <file>                Place the output into <file>\n";
  exit(1);
}

// Returns a basename without the extension from a path.
// ex) foo/bar/test.c -> test
std::string getBaseName(std::string path) {
  size_t delimPos = path.find_last_of("/");
  std::string basePath = path;
  if (delimPos != std::string::npos)
    basePath = path.substr(delimPos + 1);
  size_t dotPos = basePath.find_last_of(".");
  return dotPos == std::string::npos ? basePath
                                     : basePath.substr(0, dotPos);
}

int main(int argc, char **argv) {
  // Option and argument parsing
  if (argc < 2)
    printUsageAndExit(argv[0]);

  std::string outFile = "";
  int c = 0;
  while ((c = getopt(argc, argv,"o:")) != -1) {
    switch (c) {
    case 'o':
      outFile = optarg;
      break;
    default:
      printUsageAndExit(argv[0]);
    }
  }

  if (optind == argc) // No input file
    printUsageAndExit(argv[0]);

  std::string inFile = argv[optind];
  if (outFile.empty())
    outFile = getBaseName(inFile) + ".ll";

  // Lexing and parsing
  yyin = fopen(inFile.c_str(), "r");
  if (!yyin) {
    std::cerr << "File open error: " << argv[1] << "\n";
    exit(1);
  }

  int ret = yyparse();
  if (ret != 0) {
    std::cerr << "Parsing failed\n";
    exit(1);
  }

  // LLVM code generation
  LLVMGenerator lg(program, inFile);
  std::ofstream fout(outFile, std::ios::out | std::ios::trunc);
  if (!fout.is_open()) {
    std::cerr << "File open error: " << outFile << "\n";
    exit(1);
  }

  lg.run(fout);
  fout.close();

  delete program;

  return 0;
}
