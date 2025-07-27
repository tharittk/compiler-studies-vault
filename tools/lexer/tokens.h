#ifndef YYTOKENTYPE
# define YYTOKENTYPE

typedef union {
  int num;
  char *id;
} YYSTYPE;

enum yytokentype {
  ID = 258,      // identifier
  NUM = 259,     // number
  OR = 260,      // ||
  ASSIGN = 261,  // :=
  FUN = 262,     // fun
  IN = 263,      // in
  LET = 264,     // let
  WHILE = 265,   // while
  DO = 266,      // do
  IF = 267,      // if
  THEN = 268,    // then
  ELSE = 269,    // else
  REF = 270,     // ref
  NOT = 271,     // not
  TYPE = 272,    // type
  ARROW = 273    // ->
};
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;
