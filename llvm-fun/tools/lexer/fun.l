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
ELSE {return ELSE;}
ref {return REF;}
not {return NOT;}
type {return TYPE;}
"->" {return ARROW;}

"/*" {std::cout << "Open comment \n";} // TODO
"*/" {std::cout << "Close comment \n";}
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

{letter}+({letter}|{digit})*  {yylval.id = strdup(yytext); return ID;}
"-"?{digit}+ {yylval.num = atoi(yytext); return NUM;}
%%

void yyerror(const char *s) {
  std::cerr << yylineno << ":" << yycolumn << ": " << s << ": " << yytext
            << "\n";
}
