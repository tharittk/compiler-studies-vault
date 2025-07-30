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
%token OR ASSIGN FUN IN LET WHILE DO IF THEN ELSE REF ARROW MINUS NOT TYPE

%start program

%nonassoc IFX
%nonassoc LET IN
%nonassoc ';'
%nonassoc IF THEN ELSE DO WHILE
%left ASSIGN
%nonassoc ':'
%left '&' OR
%nonassoc NOT
%left '=' '<' '>'
%left '+' MINUS // binary minus
%left '*'
%nonassoc '#'
%right ARROW
%left REF '!' UNARY_OP
%%

unary_op: MINUS {/* do something */}
  | NOT
  | '!'
  | '#' NUM
  ;

binary_op: '+'
  | MINUS
  | '*'
  | '&'
  | OR
  | '='
  | '<'
  | '>' // not in webpage doc
  | ASSIGN
  ;

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
  | ID
  | NUM
  | exp ';' exp
  | unary_op exp %prec UNARY_OP
  | exp binary_op exp
  | '<' exp_list '>'
  | exp '(' exp ')'
  | exp ':' tp
  | if_stmt
  | WHILE exp DO exp
  | LET ID '=' exp IN exp
  | REF exp
  ;

if_stmt: IF exp THEN exp %prec IFX
  | IF exp THEN exp ELSE exp

exp_list: /* empty */
  | exp ',' exp_list // allow trailing comma
  ;


fun: FUN ID '(' ID ':' tp ')' ':' tp '=' exp;

fun_seq: /* empty */
  | fun fun_seq

program: fun_seq 

%%
