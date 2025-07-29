%{
#include "AST/AST.h"
using namespace fun;
#include "fun.tab.hpp"
#include <iostream>

void yyerror(const char *s);
int yycolumn_next = 1;
int yycolumn = 1;
int comments_depth = 0;

#define YY_USER_ACTION \
  yycolumn = yycolumn_next; \
  yycolumn_next += yyleng; \
  yylval.id = strndup(yytext, yyleng);
%}

%option yylineno
%option nounput
%option noyywrap

%x COMMENTS
/* Definitions */
letter  [A-Za-z]
digit   [0-9]
%%
[ \t] ;
fun {return FUN;}
in {return IN;}
let {return LET;}
while {return WHILE;}
do {return DO;}
if {return IF;}
then {return THEN;}
else {return ELSE;}
ref {return REF;}
not {return NOT;}
type {return TYPE;}
"->" {return ARROW;}
\( {return '(';}
\) {return ')';}
\n {yycolumn=1; yycolumn_next=1;}
"||" {return OR;}
":=" {return ASSIGN;}
\: {return ':';}
# {return '#';}
\< {return '<';}
\> {return '>';}
\+ {return '+';}
, {return ',';}
= {return '=';}
& {return '&';}
\* {return '*';}
! {return '!';}
- {return MINUS;}
";" {return ';';}

{letter}+(_|{letter}|{digit})*  {yylval.id = strdup(yytext); return ID;}
{digit}+ {yylval.num = atoi(yytext); return NUM;}

"/*" {BEGIN(COMMENTS); ++comments_depth;}
<COMMENTS>"*/" {
  --comments_depth;
  if (comments_depth == 0){
    BEGIN(INITIAL);
  }}
<COMMENTS>"/*" {++comments_depth;}
<COMMENTS>\n {yycolumn=1; yycolumn_next=1;}
<COMMENTS>. ;
. {
  fprintf(stderr, "Illegal character: '%s' at line %d, column %d\n", yytext, yylineno, yycolumn);
  exit(1);
}
%%

void yyerror(const char *s) {
  std::cerr << yylineno << ":" << yycolumn << ": " << s << ": " << yytext
            << "\n";
}
