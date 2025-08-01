%{
#include "AST/AST.h"
#include <iostream>

using namespace fun;

int yylex();
void yyerror(const char *s);
extern int yylineno;
extern int yycolumn;
extern int yyleng;

ExpASTList* exp_list = new ExpASTList();
TypeASTList* ty_list = new TypeASTList();
// ProgramAST *program = NULL;

#define SRC_LOC SrcLoc(yylineno, yycolumn)

ProgramAST *program = new ProgramAST(SRC_LOC);
%}

%union {
  int num;
  char *id;
  ExpAST* expr;
  TypeAST* ty_expr;
  FunDeclAST* fun_expr;
}
%type <expr> exp
%type <expr> primary_exp
%type <expr> if_stmt
%type <ty_expr> tp
%type <fun_expr> fun

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
%nonassoc ')'
%left FCALL

%%

tp: ID {$$ = new fun::IntTypeAST(SRC_LOC);}
  | '<' tp_list '>' {$$ = new fun::TupleTypeAST(SRC_LOC, ty_list);
                      // initialize new empty list
                      ty_list = new TypeASTList;}
  | tp ARROW tp {$$ = new fun::FunTypeAST(SRC_LOC, $1, $3);}
  | tp REF {$$ = new fun::RefTypeAST(SRC_LOC, $1);}
  | '(' tp ')' {$$ = $2;}
  ;

tp_list: /* empty */
  | tp {ty_list->append($1);}
  | tp ',' tp_list {ty_list->append($1);}
  ;

exp: primary_exp
  | exp '(' exp ')' %prec FCALL {$$ = new fun::CallExpAST(SRC_LOC, $1, $3);}
  // Unary operator
  | '-' exp %prec UNARY_OP {$$ = new fun::UnExpAST(OpKind::OP_UMinus, SRC_LOC, $2);}
  | NOT exp %prec UNARY_OP {$$ = new fun::UnExpAST(OpKind::OP_Not, SRC_LOC, $2);}
  | '!' exp %prec UNARY_OP {$$ = new fun::UnExpAST(OpKind::OP_Get, SRC_LOC, $2);}
  | '#' NUM exp %prec UNARY_OP {$$ = new fun::ProjExpAST(SRC_LOC, $2, $3);}
  | REF exp %prec UNARY_OP  {$$ = new fun::UnExpAST(OpKind::OP_Ref, SRC_LOC, $2);}
  // Binary operator
  | exp '+' exp {$$ = new fun::BinExpAST(OpKind::OP_Add, SRC_LOC, $1, $3);}
  | exp '-' exp {$$ = new fun::BinExpAST(OpKind::OP_Sub, SRC_LOC, $1, $3);}
  | exp '*' exp {$$ = new fun::BinExpAST(OpKind::OP_Mul, SRC_LOC, $1, $3);}
  | exp '&' exp {$$ = new fun::BinExpAST(OpKind::OP_And, SRC_LOC, $1, $3);}
  | exp OR exp  {$$ = new fun::BinExpAST(OpKind::OP_Or, SRC_LOC, $1, $3);}
  | exp '=' exp {$$ = new fun::BinExpAST(OpKind::OP_Equal, SRC_LOC, $1, $3);}
  | exp '<' exp {$$ = new fun::BinExpAST(OpKind::OP_LT, SRC_LOC, $1, $3);}
  | exp ASSIGN exp {$$ = new fun::BinExpAST(OpKind::OP_Set, SRC_LOC, $1, $3);}
  | exp ';' exp {$$ = new fun::SeqExpAST(SRC_LOC, $1, $3);}
  | '<' '>' {$$ = new fun::TupleExpAST(SRC_LOC, new ExpASTList);}
  | '<' non_empty_exp_list '>' {$$ = new fun::TupleExpAST(SRC_LOC, exp_list);
                                // initialize new empty list
                                exp_list = new ExpASTList;}
  | exp ':' tp {$$ = new fun::ConstrainExpAST(SRC_LOC, $1, $3);}
  | if_stmt {$$=$1;}
  | WHILE exp DO exp {$$ = new fun::WhileExpAST(SRC_LOC, /*condExp=*/$2, /*bodyExp=*/$4);}
  | LET ID '=' exp IN exp {$$ = new fun::LetExpAST(SRC_LOC, /*varName=*/$2,
                           /*varExp=*/$4, /*bodyExp=*/$6);}
  ;

primary_exp:
  '(' exp ')' {$$ = $2;}
  | ID  {$$ = new fun::IdExpAST(SRC_LOC, $1);}
  | NUM {$$ = new fun::IntExpAST(SRC_LOC, $1);}
  ;

if_stmt: IF exp THEN exp %prec IFX {$$ = new fun::IfExpAST(SRC_LOC, $2, $4);}
  | IF exp THEN exp ELSE exp {$$ = new fun::IfExpAST(SRC_LOC, $2, $4, $6);}


non_empty_exp_list:
   exp {exp_list->append($1);}
  | exp ',' non_empty_exp_list {exp_list->append($1);}
  ;


fun: FUN ID '(' ID ':' tp ')' ':' tp '=' exp {$$ = new fun::FunDeclAST(SRC_LOC, $2, $4, $6, $9, $11);}
  ;

program: /* empty */ 
  | fun program {program->append($1);}

%%
