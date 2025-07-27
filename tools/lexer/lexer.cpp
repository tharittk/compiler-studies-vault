#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

int yylex();
extern FILE *yyin;
YYSTYPE yylval;

extern int yylineno;
extern int yycolumn;

std::string tokenToStr(int token) {
  std::stringstream ss;
  switch (token) {
  case ID:
    return "ID: " + std::string(yylval.id);
  case NUM:
    ss << "NUM: " <<  yylval.num;
    return ss.str();
  case OR:
    return "||";
  case ASSIGN:
    return ":=";
  case FUN:
    return "fun";
  case IN:
    return "in";
  case LET:
    return "let";
  case WHILE:
    return "while";
  case DO:
    return "do";
  case IF:
    return "if";
  case THEN:
    return "then";
  case ELSE:
    return "else";
  case REF:
    return "ref";
  case NOT:
    return "not";
  case TYPE:
    return "type";
  case ARROW:
    return "->";
  default:
    ss << (char) token;
    return ss.str();
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " FUN_FILE\n";
    exit(1);
  }

  yyin = fopen(argv[1], "r");
  if (!yyin) {
    std::cerr << "File open error: " << argv[1] << "\n";
    exit(1);
  }

  while (true) {
    int token = yylex();
    if (token == 0) break;
    std::cout << argv[1] << ":" << yylineno << ":" << yycolumn << ": "
              << tokenToStr(token) << "\n";
  }

  return 0;
}
