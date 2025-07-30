%{
#include "AST/AST.h"

using namespace fun;

int yylex();
void yyerror(const char *s);
extern int yylineno;
extern int yycolumn;
extern int yyleng;

ProgramAST *program = NULL;

#define SRC_LOC SrcLoc(yylineno, yycolumn)
%}

%union {
  int num;
  char *id;
}
%token <num> NUM
%token <id> ID
%token OR ASSIGN FUN IN LET WHILE DO IF THEN ELSE REF ARROW NOT TYPE

%start program

%nonassoc IFX
%nonassoc LET IN
%nonassoc ';'
%nonassoc IF THEN ELSE DO WHILE
%left ASSIGN
%nonassoc ':'
%left '&' OR
%nonassoc NOT
%left '=' '<'
%left '+' '-'
%left '*'
%nonassoc '#'
%right ARROW
%left REF '!' UNARY_OP
%left FCALL

%%

tp: ID
  | '<' tp_list '>'
  | tp ARROW tp
  | tp REF
  | '(' tp ')'
  ;

tp_list: /* empty */
  | tp
  | tp ',' tp_list
  ;

exp: '(' exp ')'
  | ID '(' exp ')' %prec FCALL
  // Unary operator
  | '-' exp %prec UNARY_OP
  | NOT exp %prec UNARY_OP
  | '!' exp %prec UNARY_OP
  | '#' NUM exp %prec UNARY_OP
  // Binary operator
  | exp '+' exp
  | exp '-' exp
  | exp '*' exp
  | exp '&' exp
  | exp OR exp
  | exp '=' exp
  | exp '<' exp
  | exp ASSIGN exp
  | exp ';' exp
  | '<' '>'
  | '<' non_empty_exp_list '>'
  | exp ':' tp
  | if_stmt
  | WHILE exp DO exp
  | LET ID '=' exp IN exp
  | REF exp
  | ID
  | NUM
  ;


if_stmt: IF exp THEN exp %prec IFX
  | IF exp THEN exp ELSE exp


non_empty_exp_list:
   exp
  | exp ','
  | exp ',' non_empty_exp_list 
  ;


fun: FUN ID '(' ID ':' tp ')' ':' tp '=' exp;

program: /* empty */
  | fun program

%%
