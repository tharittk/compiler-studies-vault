#include "AST/AST.h"
#include "Visitor/CodePrinter.h"
#include "Visitor/Interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace fun;

extern ProgramAST *program;

int yylex();
int yyparse();
extern FILE *yyin;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " FUN_FILE [arg]\n";
    exit(1);
  }

  yyin = fopen(argv[1], "r");
  if (!yyin) {
    std::cerr << "File open error: " << argv[1] << "\n";
    exit(1);
  }

  // When parsing itself failed
  int ret = yyparse();
  if (ret != 0) {
    std::cerr << "Parsing failed\n";
    exit(1);
  }

  // Parsing succeeded, but AST has not been generated
  if (!program) {
    std::cerr << "Program AST has not been generated\n";
    exit(1);
  }

  std::cout << "Program:\n";
  CodePrinter cp(program, argv[1]);
  cp.run(std::cout);
  Interpreter i(program, argv[1]);
  std::cout << "\n\nRunning Program:\n";
  // The real programed program argc will be argc - 1 because we shouldn't
  // count this 'parser' executable
  MyValue* retV = i.run(argc - 1);
  std::cout << "\n\nProgram Return Value:\n";
  std::cout << retV->toString() << "\n";

  delete program;

  return 0;
}
