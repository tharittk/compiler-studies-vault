%start pascal_program

%token TIDENT
%token TUINT TUREAL TSTRING TCHAR
%token TARRAY TBEGIN TCASE TCONST TDO TDOWNTO TELSE TEND TFILE TFOR TFORWARD
%token TFUNCTION TGOTO TIF TLABEL TNEW TNIL TNOT TOF TPACKED TPROCEDURE TPROGRAM
%token TREAD TREADLN TRECORD TREPEAT TSET TTHEN TTO TTYPE TUNTIL TVAR TWHILE
%token TWITH TWRITE TWRITELN
%token TRELOP TADDOP TMULOP TSEMI TCOLON TASGN TCARET TDOT TDOTDOT TCOMMA
%token TLPAREN TRPAREN TLBRACK TRBRACK
%token TERROR

%nonassoc TRELOP /* <, >, <=, >=, =, <>, in */
%left TADDOP	 /* +, -, or */
%left TMULOP	 /* *, /, div, mod, and */
%right TNOT
%left TCOMMA

%{

#define _IN_LEX_ 

#include "IIa.h" 
#include "predefined.h" 
#include "error.msg" 

%}

%%

ident : TIDENT 
		{ Sstack.push(yylval); }
	;
unsigned_constant : TCHAR
		{ Sstack.push(yylval); 
		  Sstack.push(INTEGER_CONST_ID); }
	; 
unsigned_constant :  TUINT
		{ Sstack.push(yylval); 
		  Sstack.push(INTEGER_CONST_ID); }
	; 
unsigned_constant : TSTRING 
		{ Sstack.push(strings_table.name(yylval)); 
		  Sstack.push(STRING_CONST_ID);} 
	;
constant : unsigned_constant 
	;
constant_declaration : TCONST ident equal constant
		{ Properties	*Const; 
		  switch(Sstack.ID_pop()) { 
		    case INTEGER_CONST_ID: 
			Const = new INTEGER_CONST(Sstack.int_pop());  
			break; 
		    case STRING_CONST_ID: 
			Const = new STRING_CONST(Sstack.char_pop());  
			break; 
		    default: 
			yyerror("unresolvable constant"); 
			exit(1); 
		  } 
		  symbol_table[Sstack.int_pop()]->insert(Const);  
		} 
	;  
constant_declaration : constant_declaration TSEMI ident equal constant  
		{ Properties	*Const; 
		  switch(Sstack.ID_pop()) { 
		    case INTEGER_CONST_ID: 
			Const = new INTEGER_CONST(Sstack.int_pop());  
			break; 
		    case STRING_CONST_ID: 
			Const = new STRING_CONST(Sstack.char_pop());  
			break; 
		    default: 
			yyerror("unresolvable constant"); 
			exit(1); 
		  } 
		  symbol_table[Sstack.int_pop()]->insert(Const);  
		} 
	; 
unsigned_constant : ident 
	{ 
	  Properties *test = symbol_table[Sstack.int_pop()]->get();  
	  if (test == NULL) { 
		yyerror(EMS_id2const); 
		exit(1); 
	  } 
	  else { 
	 	test ->decompose_const(Sstack);
	  } 
	} 
	;
constant : unary_minus unsigned_constant  /* should not be a TSTRING */ 
	{ 
	  if (Sstack.ID_pop() == INTEGER_CONST_ID) { 
	  	Sstack.push(-(Sstack.int_pop())); 
		Sstack.push(INTEGER_CONST_ID); 
	  } 
	  else { 
		yyerror("inappropriate constant"); 
		exit(1); 
	  } 
	} 
	;
type_declaration : TTYPE ident equal typer
	{ TYPE	*Type = new TYPE(Sstack.type_pop()); 
	  symbol_table[Sstack.int_pop()]->insert(Type);  
	} 
	; 
type_declaration : type_declaration TSEMI ident equal typer  
	{ TYPE	*Type = new TYPE(Sstack.type_pop()); 
	  symbol_table[Sstack.int_pop()]->insert(Type);  
	} 
	; 
typer : simple_type 
	;
enumeration_list : ident
	{ Sstack.push(0); } 
	; 
enumeration_list : enumeration_list TCOMMA ident  
	{ int index = Sstack.int_pop(); 
	  int n     = Sstack.int_pop() + 1; 
	  Sstack.push(index); 
	  Sstack.push(n); 
	} 
	; 
simple_type : TLPAREN enumeration_list TRPAREN
	{ int n = Sstack.int_pop(); 
	  SCALAR *Enum = new SCALAR(n);  
	  for(int i=n ; i>=0; i--) { 
	    int index = Sstack.int_pop(); 
	    Enum->const_id_list.insert(index); 
	    symbol_table[index]->insert(new INTEGER_CONST(i));  
	  }  
	  Sstack.push(Enum); 
	} 
	; 
simple_type : constant TDOTDOT constant /* should not be TSTRINGs */
	{ int	n1,n2; 
	  ID_KIND	id1,id2; 
	  id2 = Sstack.ID_pop(); n2 = Sstack.int_pop(); 
	  id1 = Sstack.ID_pop(); n1 = Sstack.int_pop(); 
	  if ((id1 == STRING_CONST_ID) || (id2 == STRING_CONST_ID)) { 
		yyerror("inappropriate constants for a subrange"); 
	  } 
	  else { 
	  Sstack.push(new SUBRANGE(n1,n2));  
	  } 
	} 
	; 
simple_type : ident 
	{ 
	  Properties *test = symbol_table[Sstack.int_pop()]->get();  
	  if (test == NULL) { 
		test = Predefined_Boolean; 
		yyerror(EMS_id2type); 
	  } 
	  Sstack.push(test->release_type());  
	} 
	;
typer : structured_type
	; 
typer:  TCARET ident 
	{ 
	  Properties *test = symbol_table[Sstack.int_pop()]->get();  
	  if (test == NULL) { 
		test = Predefined_Boolean; 
		yyerror(EMS_id2type); 
	  } 
	  Sstack.push(new POINTER(test->release_type()));  
	} 
	;
index_list : simple_type 
	;
structured_type : TARRAY TLBRACK index_list TRBRACK TOF typer 
	{ TYPE_DESCR	*typer, *index_list; 
	  typer = Sstack.type_pop(); 
	  index_list = Sstack.type_pop(); 
	  int space = index_list->number() * typer->Size(); 
	  Sstack.push(new ARRAY(typer,index_list,space)); 
	} 
	;

variable_id_list : ident
	| variable_id_list TCOMMA ident ;
variable_declaration_part : variable_id_list TCOLON typer ;
variable : ident /* might be TINT_CONST_ID or TSTR_CONST_ID or TFUNCTION_ID */ ;
l_variable : variable ;
/* The rule
** 	expression : variable
** should logically be placed here, but there are disambiguation requirements
** which dictate that it appear AFTER certain other rules of the grammar.
*/
expression : TUINT
	| TCHAR
	| TNIL ;
addop : TADDOP ;
mulop : TMULOP ;
relop : TRELOP ;
expression : expression addop expression %prec TADDOP
	| expression mulop expression %prec TMULOP
	| expression relop expression %prec TRELOP
	| TNOT expression
	| unary_minus expression %prec TADDOP ;
subscript_list : expression ;
variable : variable TCARET
	| variable TLBRACK subscript_list TRBRACK ;

statement : if_clause true_part false_part ;
if_clause : TIF expression ;
true_part : TTHEN statement ;
false_part : TELSE statement ;
false_part : empty ;
statement : TWHILE expression TDO statement
	| TCASE expression TOF case_list TEND ;
case_list : case_label_list TCOLON statement
	| case_list TSEMI case_label_list TCOLON statement ;
case_label_list : constant /* should not be a TSTRING */
	| case_label_list TCOMMA constant ;
/* 
** The following rule MUST precede the rule
** 	expression : variable 
** in order to ensure that 'X := Y' is parsed correctly.
*/
statement : l_variable TASGN variable
	| l_variable TASGN expression
	| l_variable TASGN TSTRING ;
pascal_program : TPROGRAM ident TLPAREN program_heading TRPAREN TSEMI block TDOT ;
program_heading : ident
	| program_heading TCOMMA ident ;
statement : TNEW TLPAREN variable TRPAREN
	| TREAD TLPAREN variable TRPAREN
/* 
** The following rule MUST precede the rule
** 	expression : variable 
** in order to ensure that 'WRITE(X)' is parsed correctly.
*/
	| TWRITE TLPAREN variable TRPAREN
	| TWRITE TLPAREN expression TRPAREN
	| TWRITE TLPAREN TSTRING TRPAREN
	| TWRITELN ;

parameter_id_list : ident
	| parameter_id_list TCOMMA parameter_id_list ;
parameter_group : parameter_id_list TCOLON ident ;
formal_parameter_section : TVAR parameter_group ;
formal_parameter_list : formal_parameter_section
	| formal_parameter_list TSEMI formal_parameter_section ;
newprocid : ident ;
procedure : TPROCEDURE newprocid parameters TSEMI block TSEMI ;
newfuncid : ident ;
function : TFUNCTION newfuncid parameters TCOLON ident TSEMI block TSEMI ;
parameters : TLPAREN formal_parameter_list TRPAREN
	| empty ;
block : block1 ;
block1 : block2
	| constant_declaration TSEMI block2 ;
block2 : block3
	| type_declaration TSEMI block3 ;
block3 : block4
	| variable_declaration TSEMI block4 ;
block4 : proc_and_func TBEGIN 
		{ symbol_table.Dump(cout); } 
	statement_list TEND ;
/* 
** The following rule MUST precede the rule
** 	expression : variable 
** in order to ensure that variables in actual_parameter_lists are
** parsed correctly.
*/
actual_parameter_list : variable
	| expression
	| TSTRING
	| actual_parameter_list TCOMMA actual_parameter_list ;
statement : ident TLPAREN actual_parameter_list TRPAREN
	| ident ;
expression : ident TLPAREN actual_parameter_list TRPAREN ;

/*
** Here is the production
**	 expression : variable
** which logically might have appeared earlier, but which must be placed
** AFTER certain other rules to ensure correct disambiguation.
*/
expression : variable ;

equal : TRELOP /* restricted to = */ ;
variable_declaration : TVAR variable_declaration_part
	| variable_declaration TSEMI variable_declaration_part ;
expression : TLPAREN expression TRPAREN ;
statement : empty
	| TBEGIN statement_list TEND ;
statement_list : statement
	| statement_list TSEMI statement ;
unary_minus : TADDOP    /* restricted to - */ ;
proc_and_func : empty
	| proc_and_func procedure
	| proc_and_func function ;
empty : ;

%% 

#include "lex.++.c" 
