#include "AST/AST.h"
#include "Visitor/TypeChecker.h"
#include <stdlib.h>
#include <stdio.h>
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

  int ret = yyparse();
  if (ret != 0) {
    std::cerr << "Parsing failed\n";
    exit(1);
  }

  TypeChecker tc(program, argv[1]);
  int numError = tc.run();
  delete program;

  if (numError == 0)
    std::cout << "No error(s) has been found\n";
  else {
    std::cout << numError << " error(s) have been found\n";
    exit(1);
  }

  return 0;
}
