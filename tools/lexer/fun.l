%{
#include "tokens.h"
#include <iostream>

void yyerror(const char *s);
int yycolumn_next = 1;
int yycolumn = 1;

#define YY_USER_ACTION \
  yycolumn = yycolumn_next; \
  yycolumn_next += yyleng; \
  yylval.id = strndup(yytext, yyleng);

%}

%option yylineno
%option nounput
%option noyywrap

/* TODO Fill in definitions here */

%%

 /* TODO Fill in rules here (In the rules section a comment can't start from the
    beginning of a line) */


%%

void yyerror(const char *s) {
  std::cerr << yylineno << ":" << yycolumn << ": " << s << ": " << yytext
            << "\n";
}
