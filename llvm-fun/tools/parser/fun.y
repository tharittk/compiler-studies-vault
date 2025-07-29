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
  char *unary_op;
  char *binary_op;
  int expr;
  /* TODO Add more AST classes here */
}
%token <num> NUM
%token <id> ID
%token OR ASSIGN FUN IN LET WHILE DO IF THEN ELSE REF TYPE ARROW

%token <unary_op> MINUS NOT 
%type <expr> exp
%type <unary_op> UNARY_OP
%type <binary_op> BINARY_OP
%start program


/* TODO Fill in declarations here: union, token, type, operator associativities, ... */

%%

UNARY_OP: MINUS {/* do something */}
  | NOT
  | '!'
  | '#' NUM

BINARY_OP: '+'
  | MINUS
  | '*'
  | '&'
  | OR
  | '='
  | '<'
  | '>' // not in webpage doc
  | ASSIGN

tp: "int"
  | '<' tp '>' // TODO: n-array
  | tp ARROW tp
  | tp REF
  | '(' tp ')'


exp: '(' exp ')'
  | ID
  | NUM
  | exp ';' exp
  | UNARY_OP exp
  | exp BINARY_OP exp
  | '<' exp '>' // n-array
  | exp '(' exp ')'
  | exp ':' tp
  | IF exp THEN exp ELSE exp
  | IF exp THEN exp
  | WHILE exp DO exp
  | LET ID '=' exp IN exp
  | REF exp

fun: FUN ID '(' ID ':' tp ')' ':' tp '=' exp

main: FUN "main" '(' ID ':' tp ')' ':' tp '=' exp // TODO: both must be int

/* Fill in grammar rules and semantic actions here */

program:

%%
