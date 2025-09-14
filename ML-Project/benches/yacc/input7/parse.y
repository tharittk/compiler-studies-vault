%{
/*--------------------------------------------------------------------------
 *	parse.y
 *------------------------------------------------------------------------*/
#include <stdio.h>
#include "impact0.h"
#include "lex.h"
#include "parse.h"
#include "symbol.h"

/*	IMPORT from lex.l	*/
extern int expectTypeID;

/*	Parser state.  */
int pstate = ExitFunctDecl;

/*	Minimum checking.	*/
int structNest = 0; /* number of nested struct/union specification */ 
int defNest = 0;   /* number of nested struct/union/enum/cast specification */
int typedefNest[MAX_STRUCT_NEST];	/* number of nested typedef */
int switchNest = 0;	/* detect case and default not in switch error */
int loopNest = 0;	/* detect loop related errors */

/*	Number of syntax errors detected.  */
int parse_error = 0;	/* total number of syntax errors detected */

/*	Macro definition for arm and disarm.  */
#define ARM	(expectTypeID=1)
#define DISARM	(expectTypeID=0)

%}
%token ID, TYPE_ID, CHAR_LIT, SCALAR, LSCALAR, REAL, STRING_LIT
%token AUTO, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO
%token DOUBLE, ELSE, ENUM, EXTERN, FLOAT, FOR, GOTO, IF, INT
%token LONG, REGISTER, RETURN, SHORT, SIGNED, SIZEOF, STATIC
%token STRUCT, SWITCH, TYPEDEF, UNION, UNSIGNED, VOID, VOLATILE
%token WHILE, COMMA, SEMI, LPAR, RPAR, LBRACE, RBRACE, LSQR, RSQR
%token DOT, ARROW, PLUS, MINUS, STAR, DIV, MOD, NOT, TILDE, AND
%token OR, XOR, CONJUNCT, DISJUNCT, QUEST, COLON, RSHFT, LSHFT
%token REL_OP, EQU_OP, ASSIGN, AUTOINC, AUTODEC, ASGNOP, SHARP
%token NOALIAS
%nonassoc	IF
%nonassoc	ELSE
%%
/*
 * the following is the top level grammar description for C. It specifies 
 * what the contents of a file may be.
 */
file		:	topDeclList 
    { KillScopeAbove(0); }
		;
topDeclList	:	topDeclList topDecl 
    { ARM; }	
		|
    { ARM; }	
		;
/*----------------------------------------------------------------------------
 * Top Level Declaration.
 * precondition : armed.
 * postcondition : armed.
 */
topDecl		:	globalDataDecl
    { 	
	Process_Parse_Tree($1); 
	$$ = nil;
    }
		|	functDecl
    { 
	Process_Parse_Tree($1); 
	KillScopeAbove(1);
	$$ = nil;
    }
		|	SEMI
    {
	Process_Parse_Tree(nil);
	$$ = nil;
    }
		|	error
    {
	parseError("unrecoverable syntax error");
	print_tree($0, 2, 'E');
	exit(0);
    }
		;
/*
 * fixes for expectTypeID.
 */
arm		:
    { ARM; $$ = nil; }
		;
disarm		:
    { DISARM; $$ = nil; }
		;
/*----------------------------------------------------------------------------
 * 	Grammar for function definitions
 * precondition : armed.
 * postcondition : armed.
 */
functDecl	:	functDefine LBRACE stmtBody arm RBRACE
    { 
	pstate = ExitFunctDecl;
	DoScopeOut();
	$$ = MakeNtNode(T_functDecl, "functDecl", $1, $3, nil); 
    }
		;
/* 7/2/1988 : add this rule to handle parameter definition.
 * need to create new symbol for parameters defined in function dcltr,
 * but not defined in prmDecl.
 */
functDefine	:	functHead prmDecl
    {
	DefineParamID();
	$$ = Concat($1, $2);
    }
		;
/*
 * precondition : armed.
 * postcondition : armed. (dcltr enforced)
 */
functHead	:	optDeclSpec dcltr 
    { 
	/* 	Entering function body definition.	*/
	pstate = EnterFunctDecl;
	/*	Create a new scope.	*/
	DoScopeIn();
	$$ = MakeNtNode(T_functHead, "functHead", $1, $2, nil); 
    }
		;
/*
 * precondition : armed.
 * postcondition : armed.
 */
prmDecl		:	prmDecl paramDecl 
    { ARM; $$ = Concat($1, $2); }
		|	
    { ARM; $$ = nil; }
		;
paramDecl	:	declSpec arm SEMI
    { 
	/*	Reset the typedefNest counter for the current level.  */
	typedefNest[defNest] = 0;
	$$ = MakeNtNode(T_paramDataDecl, "paramDataDecl", $1, nil, nil); 
    }
		|	declSpec listDcltr arm SEMI
    { 
	/*	Reset the typedefNest counter for the current level.  */
	typedefNest[defNest] = 0;
	$$ = MakeNtNode(T_paramDataDecl, "paramDataDecl", $1, $2, nil); 
    }
		;
/*----------------------------------------------------------------------------
 * 	Grammar for statements
 */
/*
 * precondition : none.
 * postcondition : disarmed.
 */
compoundStmt	:	compoundHead stmtBody RBRACE 
    { 
	DoScopeOut(); 		/* return to the previous scope */
	DISARM;			/* postcondition : disarmed */
	$$ = MakeNtNode(T_compoundStmt, "compoundStmt", $2, nil, nil); 
    }
		;
/* precondition : none.
 * postcondition : armed.
 */
compoundHead	:	LBRACE
    { 
	DoScopeIn(); 
	ARM;
	$$ = nil;
    }
		;
/* precondition : armed.
 * postcondition : armed.
 */
stmtBody	:	localDeclList localStmtList
    { ARM; $$ = Concat($1, $2); }
		|	localDeclList
    { ARM; $$ = $1; }
		|	localStmtList
    { ARM; $$ = $1; }
		|
    { ARM; $$ = nil; }
		;
localDeclList	:	localDeclList localDataDecl
    { ARM; $$ = Concat($1, $2); }
		|	localDataDecl
    { ARM; $$ = $1; }
		;
localStmtList	:	localStmtList stmt
    { ARM; $$ = Concat($1, $2); }
		|	stmt
    { ARM; $$ = $1; }
		;
/*
 * Since within stmt, all identifiers are of type ID, and since
 * all IDs must be declared before use, we can be sure that there
 * are no ambiquity between TYPE_ID and IDs here.
 * For label names, we require all label names to be different from
 * all defined IDs and TYPE_IDs.
 * For function names which may be used before define, we
 * restrict the use of the same name for function and typeid
 * in the same lexical scope.
 *
 * precondition : none. 
 * postcondition : disarmed.
 */
stmt		:	basicStmt
    { $$ = $1; }
		|	WhileStmt
    { $$ = $1; }
		|	ForStmt
    { $$ = $1; }
		|	IfStmt
    { $$ = $1; }
		|	switchStmt
    { $$ = $1; }
		;
/* 
 * grammar for normal types of statements
 */
/* precondition : none.
 * postcondition : don't know.
 */
basicStmt	:	exprList SEMI
    { $$ = $1; }
		|	compoundStmt
    { $$ = $1; }
		|	doHead doBody
    { $$ = $2; }
		|	BREAK SEMI
    { 
	if (loopNest == 0 && switchNest == 0) {
		parseError("break statement not inside a loop or switch");
	}
	$$ = MakeNtNode(T_break, "break", nil, nil, nil); 
    }
		|	CONTINUE SEMI
    { 	
	if (loopNest == 0) {
		parseError("continue statement not inside a loop");
	}
	$$ = MakeNtNode(T_continue, "continue", nil, nil, nil); 
    }
		|	RETURN SEMI
    { $$ = MakeNtNode(T_return, "return", nil, nil, nil); }
		|	RETURN exprList SEMI
    { $$ = MakeNtNode(T_return, "return", $2, nil, nil); }
		|	GOTO label_ID SEMI
    { $$ = MakeNtNode(T_goto, "goto", $2, nil, nil); }
		|	label_ID COLON stmt
    { $$ = Concat(MakeNtNode(T_label, "label", $1, nil, nil), $3); }
		| 	caseHead stmt
    { 
	/*
	 *	This rule will overflow when there is a long chain
	 *	of case : case : ... statements.
	 */
	$$ = InsertRightMost($1, $2);
    }
		|	DEFAULT COLON stmt
    { 
	if (switchNest == 0) {
		parseError("default not in switch statement");
	}
	$$ = MakeNtNode(T_default, "default", nil, $3, nil); 
    }
		|	SEMI
    { 
	/* 
	 *	We need to generate a noop node for this, because we
	 *	may have a label in front of this instruction, which
	 *	may be branched to later in the program.
	 */
	$$ = MakeNtNode(T_noop, "noop", nil, nil, nil); 
    }
		;
caseHead	:	CASE exprOnly COLON
    {
	if (switchNest == 0) {
		parseError("case not in switch statement");
	}
	$$ = MakeNtNode(T_case, "case", $2, nil, nil); 
    }
		;
doHead		:	DO
    { loopNest++; $$ = nil; }		
		;
doBody		:	stmt WHILE LPAR exprList RPAR SEMI
    { 
	loopNest--;
	$$ = MakeNtNode(T_do_while, "do_while", $1, $4, nil); 
    }
		;
/*
 *	There is a shift/reduce error here.
 *	But it will work anyway.
 */
IfStmt		:	IfHead IfBody			%prec IF
    { $$ = MakeNtNode(T_if, "if", $1, $2, nil); }
		| 	IfHead IfBody ELSE stmt		%prec ELSE
    { $$ = MakeNtNode(T_if, "if", $1, 
		MakeNtNode(T_else, "else", $2, $4, nil), nil); 
    }
		;
IfHead		:	IF LPAR exprList RPAR
    {	$$ = $3; }
		;
IfBody		:	stmt
    {	$$ = $1; }
		;
WhileStmt	:	whileHead exprList RPAR stmt
    {	
	loopNest--;
	$$ = MakeNtNode(T_while, "while", $2, $4, nil); 
    }
		;
whileHead	:	WHILE LPAR 
    {	
	loopNest++;
	$$ = nil;
    }
		;
ForStmt		:	forHead stmt
    {
	loopNest--;
	$$ = MakeNtNode(T_for, "for", $1, $2, nil); 
    }
		;
forHead		:	forInit forEnable
    { $$ = MakeNtNode(T_for_init, "for_init", $1, $2, nil); }
		;
forInit		:	FOR LPAR
    { loopNest++; $$ = nil; }
		|	FOR LPAR exprList
    { loopNest++; $$ = $3; }
 		;
forEnable	:	SEMI forIterate
    { $$ = MakeNtNode(T_for_enable, "for_enable", nil, $2, nil); }
		|	SEMI exprList forIterate
    { $$ = MakeNtNode(T_for_enable, "for_enable", $2, $3, nil); }
		;
forIterate	:	SEMI RPAR
    { $$ = MakeNtNode(T_for_iterate, "for_iterate", nil, nil, nil); }
		|	SEMI exprList RPAR
    { $$ = MakeNtNode(T_for_iterate, "for_iterate", $2, nil, nil); }
		;
switchStmt	:	switchHead stmt
    { switchNest--;
	$$ = MakeNtNode(T_switch, "switch", $1, $2, nil); 
    }
		;
switchHead	:	SWITCH LPAR exprList RPAR
    { switchNest++; $$ = $3; }
		;
/*
 * the grammar for data declarations.
 */
globalDataDecl	:	declSpec arm SEMI
    {
	/*	Reset the typedefNest counter for the current level.  */
	typedefNest[defNest] = 0;
	$$ = MakeNtNode(T_globalDataDecl, "globalDataDecl", $1, nil, nil); 
    }
		|	optDeclSpec listInitDcltr arm SEMI
    { 
	/*	Reset the typedefNest counter for the current level.  */
	typedefNest[defNest] = 0;
	$$ = MakeNtNode(T_globalDataDecl, "globalDataDecl", $1, $2, nil); 
    }
		;
localDataDecl	:	declSpec arm SEMI
    { 
	/*	Reset the typedefNest counter for the current level.  */
	typedefNest[defNest] = 0;
	$$ = MakeNtNode(T_localDataDecl, "localDataDecl", $1, nil, nil); 
    }
		|	declSpec listInitDcltr arm SEMI
    { 
	/*	Reset the typedefNest counter for the current level.  */
	typedefNest[defNest] = 0;
	$$ = MakeNtNode(T_localDataDecl, "localDataDecl", $1, $2, nil); 
    }
		;
/*-----------------------------------------------------------------------*/
/*
 * precondition : armed.
 * postcondition : disarmed.
 */
optDeclSpec	:	declSpec 	
    {	/* We must enter optDeclSpec already in armed state.  */
	$$ = $1; 
    }
		|		
    {	DISARM; $$ = nil; }
		;
/*
 * precondition : armed.
 * postcondition : disarmed.
 */
declSpec	:	tcList 
    {	/* We must enter declSpec already in armed state.  */
	DISARM; $$ = $1; 
    }
		;
tcList		:	tcSpec
    {	$$ = $1; }
		|	tcList tcSpec
    {	$$ = Concat($1, $2); }
		;
tcSpec		:	stdClass
    {	$$ = $1; }
		|	typeSpec
    {	/*	After a type is given, we no longer expect an
	 *	user defined type. Be careful that we need to
	 *	arm before returning to the topDecl level.
	 */
	expectTypeID = 0;
	$$ = $1; 
    }
		;
stdClass	:	AUTO		
    { $$ = MakeNtNode(T_auto, "auto", nil, nil, nil); }
		|	STATIC		
    { /*	static is not allowed in struct/union definition.	*/
	if (structNest > 0)
		parseError("static class is not allowed in struct/union");
	$$ = MakeNtNode(T_static, "static", nil, nil, nil); 
    }
		|	EXTERN	
    { /*	extern is not allowed in struct/union definition.	*/
	if (structNest > 0)
		parseError("extern class is not allowed in struct/union");
	$$ = MakeNtNode(T_extern, "extern", nil, nil, nil); 
    }
		|	REGISTER
    { /*	register is not allowed in struct/union definition.	*/
	if (structNest > 0)
		parseError("register class is not allowed in struct/union");
	$$ = MakeNtNode(T_register, "register", nil, nil, nil); 
    }
		|	CONST
    { /*	const is not allowed in struct/union definition.	*/
	if (structNest > 0)
		parseError("const qualifier is not allowed in struct/union");
	$$ = MakeNtNode(T_const, "const", nil, nil, nil); 
    }
		|	NOALIAS
    { /*	noalias is not allowed in struct/union definition.	*/
	if (structNest > 0)
		parseError("noalias qualifier is not allowed in struct/union");
	$$ = MakeNtNode(T_noalias, "noalias", nil, nil, nil); 
    }
		|	VOLATILE
    { /*	volatile is not allowed in struct/union definition.	*/
	if (structNest > 0)
		parseError("volatile qualifier is not allowed in struct/union");
	$$ = MakeNtNode(T_volatile, "volatile", nil, nil, nil); 
    }
		|	TYPEDEF
    { /*	typedef is not allowed in struct/union definition.	*/
	if (structNest > 0)
		parseError("typedef class is not allowed in struct/union");
	/*
	 *	Found another typedef in the current decl.
	 */
	typedefNest[defNest]++;
	if (typedefNest[defNest] > 1) {
		parseError("too many occurances of typedef");
	}
	$$ = MakeNtNode(T_typedef, "typedef", nil, nil, nil); 
    }
		;
typeSpec	:	usrType	
    {	$$ = $1; }
		|	stdType
    {	$$ = $1; }
		|	enumSpec
    { $$ = $1; }
		|	structSpec
    {	$$ = $1; }
		;
usrType		:	type_ID
    { $$ = $1; }
		;
stdType		:	CHAR	
    { $$ = MakeNtNode(T_char, "char", nil, nil, nil); }
		|	FLOAT
    { $$ = MakeNtNode(T_float, "float", nil, nil, nil); }
		|	DOUBLE
    { $$ = MakeNtNode(T_double, "double", nil, nil, nil); }
		|	INT	
    { $$ = MakeNtNode(T_int, "int", nil, nil, nil); }
		|	SHORT
    { $$ = MakeNtNode(T_short, "short", nil, nil, nil); }
		|	LONG
    { $$ = MakeNtNode(T_long, "long", nil, nil, nil); }
		|	SIGNED
    { $$ = MakeNtNode(T_signed, "signed", nil, nil, nil); }
		|	UNSIGNED
    { $$ = MakeNtNode(T_unsigned, "unsigned", nil, nil, nil); }
		|	VOID
    { $$ = MakeNtNode(T_void, "void", nil, nil, nil); }
		;
enumSpec	:	enumHead disarm enumTag
    { /*	struct/union/enum checking	*/
	structNest--;
	defNest--;
	$$ = MakeNtNode(T_enum, "enum", $3, nil, nil); 
    }
		|	enumHead arm fieldHead enumList RBRACE
    { /*	struct/union/enum checking	*/
	structNest--;
	defNest--;
	/*	variable/typeid scope management	*/
	DoScopeOut();
	$$ = MakeNtNode(T_enum, "enum", nil, $4, nil); 
    }
		|	enumHead disarm enumTag arm fieldHead enumList RBRACE
    { /*	struct/union/enum checking	*/
	structNest--;
	defNest--;
	/*	variable/typeid scope management	*/
	DoScopeOut();
	$$ = MakeNtNode(T_enum, "enum", $3, $6, nil); 
    }
		;
enumHead	:	ENUM
    {
	DISARM;		/* for enum tag */
	/*
	 *	Set up checking information for a new level
	 *	of struct/union/enum definition.
	 */
	structNest++;
	defNest++;
	if (defNest >= MAX_STRUCT_NEST) {
		parseError("too many levels of struct/union/enum definition");
		exit(0);
	}
	/*	Reset the type declaration.	*/
	typedefNest[defNest] = 0;
	$$ = nil;
    }
		;
enumTag	: 	enum_ID
    { ARM; $$ = $1; }
		;
enumList	:	enumDcltr
    {	$$ = $1; }
		|	enumList COMMA enumDcltr
    { $$ = Concat($1, $3); }
		;
enumDcltr	:	field_ID
    { $$ = MakeNtNode(T_enumItem, "enumItem", $1, nil, nil); }
		|	field_ID ASSIGN exprOnly
    { $$ = MakeNtNode(T_enumItem, "enumItem", $1, $3, nil); }
		;
structSpec	:	structHead disarm structTag 
    { /*	struct/union checking	*/
	structNest--;
	defNest--;
	$$ = MakeNtNode(T_struct, "struct", $3, nil, nil); 
    }
		|	structHead arm fieldHead fieldList RBRACE
    { /*	struct/union checking	*/
	structNest--;
	defNest--;
	/*	variable/typeid scope management	*/
	DoScopeOut();
	$$ = MakeNtNode(T_struct, "struct", nil, $4, nil); 
    }
		|	structHead disarm structTag arm fieldHead fieldList RBRACE
    { /*	struct/union checking	*/
	structNest--;
	defNest--;
	/*	variable/typeid scope management	*/
	DoScopeOut();
	$$ = MakeNtNode(T_struct, "struct", $3, $6, nil); 
    }
		|	unionHead disarm unionTag 
    { /*	struct/union checking	*/
	structNest--;
	defNest--;
	$$ = MakeNtNode(T_union, "union", $3, nil, nil); 
    }
		|	unionHead arm fieldHead fieldList RBRACE
    { /*	struct/union checking	*/
	structNest--;
	defNest--;
	/*	variable/typeid scope management	*/
	DoScopeOut();
	$$ = MakeNtNode(T_union, "union", nil, $4, nil); 
    }
		|	unionHead disarm unionTag arm fieldHead fieldList RBRACE
    { /*	struct/union checking	*/
	structNest--;
	defNest--;
	/*	variable/typeid scope management	*/
	DoScopeOut();
	$$ = MakeNtNode(T_union, "union", $3, $6, nil); 
    }
		;
structHead	:	STRUCT
    {
	DISARM;		/* for struct tag */
	/*
	 *	Set up checking information for a new level
	 *	of struct/union definition.
	 *	Initialize all type keeping information for
	 *	the new structNest level.
	 */
	structNest++;
	defNest++;
	if (defNest >= MAX_STRUCT_NEST) {
		parseError("too many levels of struct/union/cast definition");
		exit(0);
	}
	/*	Reset the type declaration.	*/
	typedefNest[defNest] = 0;
	$$ = nil;
    }
		;
unionHead	:	UNION
    {
	DISARM;		/* for union tag */
	/*
	 *	Set up checking information for a new level
	 *	of struct/union definition.
	 */
	structNest++;
	defNest++;
	if (defNest >= MAX_STRUCT_NEST) {
		parseError("too many levels of struct/union definition");
		exit(0);
	}
	/*	Reset the type declaration.	*/
	typedefNest[defNest] = 0;
	$$ = nil;
    }
		;
structTag	: 	struct_ID
    { ARM; $$ = $1; }
		;
unionTag	:	union_ID
    { ARM; $$ = $1; }
		;
fieldHead	:	LBRACE 
    { ARM; DoScopeIn(); $$ = nil; }
		;
fieldList	:	fieldList fieldDecl arm
    { $$ = Concat($1, $2); }
		|	fieldDecl arm
    { $$ = $1; }
		;
fieldDecl	:	declSpec disarm fieldNameList arm SEMI
    { /*	first clear the expectTypeID option so the bit
	 *	field code does not get upset about prior dcltrs.
	 */
	/*	Reset the type declaration.	*/
	typedefNest[defNest] = 0;

	$$ = MakeNtNode(T_fieldDecl, "fieldDecl", $1, $3, nil); 
    }
		;
fieldNameList	:	fieldDcltr
    {	$$ = $1; }
		|	fieldNameList COMMA fieldDcltr
    { $$ = Concat($1, $3); }
		;
fieldDcltr	:	dcltr
    { $$ = MakeNtNode(T_fieldItem, "fieldItem", $1, nil, nil); }
		|	COLON exprOnly
    { $$ = MakeNtNode(T_fieldItem, "fieldItem", nil, $2, nil); }
		|	dcltr COLON exprOnly
    { $$ = MakeNtNode(T_fieldItem, "fieldItem", $1, $3, nil); }
		;
/*
 * 	Initializable variables. (global, local)
 */
listInitDcltr	:	initList
    {	$$ = $1; }
		;
initList	:	initDcltr
    {	$$ = $1; }
		|	initList COMMA initDcltr
    {	$$ = Concat($1, $3); }
		;
/*
 *	Non-initializable variables. (fields, typedef, parameters)
 */
listDcltr	:	dcltrList
    {	$$ = $1; }
		;
dcltrList	:	dcltr
    {	$$ = $1; }
		|	dcltrList COMMA dcltr
    { $$ = Concat($1, $3); }
		;
/*
 *	The famed C syntax for declarators. 
 */
/*
 *	Variable and typeids are not compatible.
 *	It is an error that a declarator has been
 *	defined as typeid in the current level.
 *
 * precondition : don't care
 * postcondition : armed
 */
dcltr		:	disarm p3Dcltr 
    { $$ = MakeNtNode(T_dcltr, "dcltr", $2, nil, nil); }
		;
p3Dcltr		:	p2Dcltr
    { $$ = $1; }
		|	STAR p3Dcltr
    { $$ = MakeNtNode(T_starDcltr, "*", nil, nil, $2); }
		;
p2Dcltr		:	p1Dcltr 
    { /*	p1Dcltr must be an undefined variable/user_define_type.
	 *	So we disarm expectTypeID option before calling
	 *	p1Dcltr.
	 */
	$$ = $1; 
    } 
		|	p2Dcltr disarm LPAR formalsDecl arm RPAR	
    { $$ = MakeNtNode(T_funcDcltr, "funcDcltr", $4, nil, $1); }
		|	p2Dcltr LSQR RSQR
    { $$ = MakeNtNode(T_arryDcltr, "arryDcltr", nil, nil, $1); }
		|	p2Dcltr LSQR exprList RSQR
    { $$ = MakeNtNode(T_arryDcltr, "arryDcltr", $3, nil, $1); }
		;
p1Dcltr		:	define_ID
    { ARM; $$ = $1; }
		|	LPAR p3Dcltr RPAR
    { ARM; $$ = MakeNtNode(T_enclosure, "enclosure", nil, nil, $2); }
		;
/*
 * formal parameters to functions
 * precondition : disarmed.
 * postcondition : disarmed.
 */
formalsDecl	:	formalDecl 
    { 
	/*
	 *	Parameters must be ID, so we disarm expectTypeID.
	 */
	$$ = $1;
    }
		|
    { $$ = nil; }
		;
formalDecl	:	param_ID
    { $$ = $1; }
		|	formalDecl COMMA param_ID
    { $$ = Concat($1, $3); }
		;
/*
 * syntax for initialized variables
 */
initDcltr	:	dcltr
    { $$ = $1; }
		|	dcltr ASSIGN initExpr
    { /*
	 *	Typeid can not be initialized.
	 */
	if (typedefNest[defNest] > 0) {
		parseError("can not initialize typeid");
	}
     	RightOf($1) = $3;
     	$$ = $1;
    }
		;
initExpr	:	exprOnly
    { $$ = $1; }
		|	LBRACE initExprList RBRACE
    { $$ = MakeNtNode(T_brace, "brace", $2, nil, nil); }
		|	LBRACE initExprList COMMA RBRACE
    { $$ = MakeNtNode(T_brace, "brace", $2, nil, nil); }
		;
initExprList	:	initExpr
    { $$ = $1; }
		|	initExprList COMMA initExpr
    { $$ = Concat($1, $3); }
		;
/*
 * abstract declarators
 */
absDcltr	:	p2AbsDcltr
    { $$ = $1; }
		|
    { $$ = nil; }
		;
p2AbsDcltr	:	STAR p2AbsDcltr
    { $$ = MakeNtNode(T_starDcltr, "p2AbsDcltr", nil, nil, $2); }
		|	STAR
    { $$ = MakeNtNode(T_starDcltr, "p2AbsDcltr", nil, nil, nil); }
		|	p1AbsDcltr
    { $$ = $1; }
		;
p1AbsDcltr	:	p1AbsDcltr LPAR RPAR
    { $$ = MakeNtNode(T_funcDcltr, "p1AbsDcltr", nil, nil, $1); }
		|	p1AbsDcltr LSQR RSQR
    { $$ = MakeNtNode(T_arryDcltr, "p1AbsDcltr", nil, nil, $1); }
		|	p1AbsDcltr LSQR exprList RSQR
    { $$ = MakeNtNode(T_arryDcltr, "p1AbsDcltr", $3, nil, $1); }
		|	LPAR RPAR
    { $$ = MakeNtNode(T_funcDcltr, "p1AbsDcltr", nil, nil, nil); }
		|	LSQR RSQR
    { $$ = MakeNtNode(T_arryDcltr, "p1AbsDcltr", nil, nil, nil); }
		|	LSQR exprList RSQR
    { $$ = MakeNtNode(T_arryDcltr, "p1AbsDcltr", $2, nil, nil); }
		|	LPAR p2AbsDcltr RPAR
    { /* * This node is required for reverse translation.
	 */
	$$ = MakeNtNode(T_enclosure, "p2AbsDcltr", nil, nil, $2); 
    }
		;
/*---------------------------------------------------------------------------
 * Id nodes.
 *	Other than TYPE_ID nodes, the lexer returns ID.
 *	It is up to the parser here to define them distinctively.
 *	Define a new variable or typeid.
 *	e.g. int x; typedef double y;
 */
define_ID       :       ID 
    { 
        /*
         *      It is an error if the symbol has already been 
         *      defined in the current lexical scope. 
         *      Except when it was declared as extern variables. 
         *      typeid has been declared as a part of decl, when
         *      typedefNest[currentStructNest]>0. 
         */ 
        if (typedefNest[defNest] > 0) { 
                $$ = MakeNewTypeIDNode(); 
        } else { 
                $$ = MakeNewIDNode();
        } 
    } 
                ; 
/*	goto label_ID;
 *	label_ID : stmt;
 */
label_ID	:	ID
    { $$ = MakeLabel(T_id, "label"); }
		;
/*	user_define_type x; */
type_ID		:	TYPE_ID
    { $$ = MakeTypeID(); } 
		;
/*	parameter variable definition */
param_ID	:	ID
    { $$ = MakeParamID(); }
/*	variable used in expression */
var_ID		:	ID
    { $$ = MakeVarID(); } 
		;
/*	struct/union/enum field variable name */
field_ID	:	ID
    { $$ = MakeFieldID(); } 
		;
/* enum tag name. */
enum_ID		:	ID
    { $$ = MakeEnumTag(); }
		;
/*	struct tag name */
struct_ID	:	ID
    { $$ = MakeStructTag(); }
		;
/*	union tag name */
union_ID	:	ID
    { $$ = MakeUnionTag(); }
		;
/*--------------------------------------------------------------------------*/
/*
 *	All variables used in expr are IDs. There is no
 *	ambiquity with TYPEID because variables must be defined
 *	before they are used. It is an error if a TYPEID is found
 *	here in the place of an ID.
 *
 * precondition : none.
 * postcondition : disarmed.
 */
exprList	:	listExpr
    {
	DISARM;
	/* *	This is used strictly by rules shown above (stmts, decls) */
	$$ = MakeNtNode(T_expression, "expression", $1, nil, nil); 
    }
		;
/* precondition : none.
 * postcondition : disarmed.
 */
exprOnly	:	expr
    {
	DISARM;
	/* *	This is used strictly by rules shown above (stmts, decls) */
	$$ = MakeNtNode(T_expression, "expression", $1, nil, nil); 
    }
		;
/* ----------------------------------------------------------------------------
 * The syntax for expressions.
 */
/*
 * precondition : don't care
 * postcondition : armed.
 */
listExpr	:	expr
    { $$ = $1; }
		|	listExpr COMMA expr
    { $$ = MakeNtNode(T_comma, ",", $1, $3, nil); }
		;
/* precondition : don't care
 * postcondition : armed.
 */
expr		:	assignExpr
    { ARM; $$ = $1; }
		;
assignExpr	:	condExpr
    { $$ = $1; }
		|	condExpr ASSIGN expr
    { $$ = MakeNtNode(T_assign, "=", $1, $3, nil); }
		|	condExpr asgnopOpcode expr
    { LeftOf($2) = $1;
	RightOf($2) = $3;
	$$ = $2;
    }
		;
asgnopOpcode	:	ASGNOP
    {
	if (TokenClass==TAplus) {
		$$ = MakeNtNode(T_Aplus, "+=", nil, nil, nil); 
	} else
	if (TokenClass==TAminus) {
		$$ = MakeNtNode(T_Aminus, "-=", nil, nil, nil); 
	} else
	if (TokenClass==TAstar) {
		$$ = MakeNtNode(T_Astar, "*=", nil, nil, nil); 
	} else
	if (TokenClass==TAdiv) {
		$$ = MakeNtNode(T_Adiv, "/=", nil, nil, nil); 
	} else
	if (TokenClass==TAmod) {
		$$ = MakeNtNode(T_Amod, "%=", nil, nil, nil); 
	} else
	if (TokenClass==TArshft) {
		$$ = MakeNtNode(T_Arshft, ">>=", nil, nil, nil); 
	} else
	if (TokenClass==TAlshft) {
		$$ = MakeNtNode(T_Alshft, "<<=", nil, nil, nil); 
	} else
	if (TokenClass==TAand) {
		$$ = MakeNtNode(T_Aand, "&=", nil, nil, nil); 
	} else
	if (TokenClass==TAor) {
		$$ = MakeNtNode(T_Aor, "|=", nil, nil, nil); 
	} else
	if (TokenClass==TAxor) {
		$$ = MakeNtNode(T_Axor, "^=", nil, nil, nil); 
	} else {
		parseError("illegal ASGNOP operation");
		$$ = MakeNtNode(T_noop, "error", nil, nil, nil);
	}
    }
		;
/* precondition : armed.
 * postcondition : armed.
 */
condExpr	:	orOperExpr
    { $$ = $1; }
		|	orOperExpr QUEST listExpr arm COLON condExpr
    { $$ = MakeNtNode(T_quest, "?", $1, 
		MakeNtNode(T_colon, ":", $3, $6, nil), nil); 
    }
		;
orOperExpr	:	andOperExpr
    { $$ = $1; }
		|	orOperExpr DISJUNCT andOperExpr
    { $$ = MakeNtNode(T_disj, "||", $1, $3, nil); }
		;
andOperExpr	:	bitorOperExpr
    { $$ = $1; }
		|	andOperExpr CONJUNCT bitorOperExpr
    { $$ = MakeNtNode(T_conj, "&&", $1, $3, nil); }
		;
bitorOperExpr	:	bitxorOperExpr
    { $$ = $1; }
		|	bitorOperExpr OR bitxorOperExpr
    { $$ = MakeNtNode(T_or, "|", $1, $3, nil); }
		;
bitxorOperExpr	:	bitandOperExpr
    { $$ = $1; }
		|	bitxorOperExpr XOR bitandOperExpr
    { $$ = MakeNtNode(T_xor, "^", $1, $3, nil); }
		;
bitandOperExpr	:	equOperExpr
    { $$ = $1; }
		|	bitandOperExpr	AND equOperExpr
    { $$ = MakeNtNode(T_and, "&", $1, $3, nil); }
		;
equOperExpr	:	relOperExpr
    { $$ = $1; }
		|	equOperExpr eqOpcode relOperExpr
    { LeftOf($2) = $1;
	RightOf($2) = $3;
	$$ = $2;
    }
		;
eqOpcode	:	EQU_OP
    { if (TokenClass==Teq) {
		$$ = MakeNtNode(T_eq, "==", nil, nil, nil); 
	} else
	if (TokenClass==Tne) {
		$$ = MakeNtNode(T_ne, "!=", nil, nil, nil); 
	} else {
		parseError("illegal EQU_OP");
		$$ = MakeNtNode(T_noop, "error", nil, nil, nil);
	}
    }
		;
relOperExpr	:	shiftOperExpr
    { $$ = $1; }
		|	relOperExpr relOpcode shiftOperExpr
    { 	
	LeftOf($2) = $1;
	RightOf($2) = $3;
	$$ = $2;
    }
		;
relOpcode	:	REL_OP
    {
	/*
	 * Find out which relational operation this is really is.
	 */
	if (TokenClass==Tlt) {
		$$ = MakeNtNode(T_lt, "<", nil, nil, nil);
	} else
	if (TokenClass==Tle) {
		$$ = MakeNtNode(T_le, "<=", nil, nil, nil);
	} else
	if (TokenClass==Tge) {
		$$ = MakeNtNode(T_ge, ">=", nil, nil, nil);
	} else
	if (TokenClass==Tgt) {
		$$ = MakeNtNode(T_gt, ">", nil, nil, nil);
	} else {
		parseError("illegal REL_OP");
		$$ = MakeNtNode(T_noop, "error", nil, nil, nil);
	}
    }
		;
shiftOperExpr	:	addOperExpr
    { $$ = $1; }
		|	shiftOperExpr RSHFT addOperExpr
    { $$ = MakeNtNode(T_rshft, ">>", $1, $3, nil); }
		|	shiftOperExpr LSHFT addOperExpr
    { $$ = MakeNtNode(T_lshft, "<<", $1, $3, nil); }
		;
addOperExpr	:	multOperExpr
    { $$ = $1; }
		|	addOperExpr PLUS multOperExpr
    { $$ = MakeNtNode(T_add, "+", $1, $3, nil); }
		|	addOperExpr MINUS multOperExpr
    { $$ = MakeNtNode(T_sub, "-", $1, $3, nil); }
		;
multOperExpr	:	castExpr
    { $$ = $1; }
		|	multOperExpr STAR castExpr
    { $$ = MakeNtNode(T_mul, "*", $1, $3, nil); }
		|	multOperExpr DIV castExpr
    { $$ = MakeNtNode(T_div, "/", $1, $3, nil); }
		|	multOperExpr MOD castExpr
    { $$ = MakeNtNode(T_mod, "%", $1, $3, nil); }
		;
/* precondition : armed.
 * postcondition : armed.
 */
castExpr	:	prefixExpr
    { $$ = $1; }
		|	castHeadHead castHead RPAR castExpr
    { $$ = MakeNtNode(T_cast, "cast", $2, $4, nil); }
		;
castHead	:	declSpec absDcltr disarm
    { defNest--;
	$$ = MakeNtNode(T_castHead, "castHead", $1, $2, nil); 
    }
		;
castHeadHead	:	leftPAR
    { /*
	 *	Set up checking information for a new level
	 *	of struct/union/cast definition.
	 *	Initialize all type keeping information for
	 *	the new cast level.
	 */
	defNest++;
	if (defNest >= MAX_STRUCT_NEST) {
		parseError("too many levels of struct/union/cast definition");
		exit(0);
	}
	/*	Reset the type declaration.	*/
	typedefNest[defNest] = 0;
	$$ = nil;
    }
		;
/* precondition : armed.
 * postcondition : armed.
 */
prefixExpr	:	postfixExpr
    { $$ = $1; }
		|	SIZEOF prefixExpr
    { $$ = MakeNtNode(T_sizeof, "sizeof", $2, nil, nil); }
		|	AUTOINC castExpr
    { $$ = MakeNtNode(T_preinc, "pre++", $2, nil, nil); }
		|	AUTODEC castExpr
    { $$ = MakeNtNode(T_predec, "pre--", $2, nil, nil); }
		|	STAR castExpr
    { $$ = MakeNtNode(T_indr, "*", $2, nil, nil); }
		|	AND castExpr
    { $$ = MakeNtNode(T_addr, "&", $2, nil, nil); }
		|	MINUS castExpr
    { $$ = MakeNtNode(T_neg, "-", $2, nil, nil); }
		|	NOT castExpr
    { $$ = MakeNtNode(T_not, "!", $2, nil, nil); }
		|	TILDE castExpr
    { $$ = MakeNtNode(T_inv, "~", $2, nil, nil); }
		;
/* precondition : armed.
 * postcondition : armed.
 */
postfixExpr	:	primaryP1Expr
    { $$ = $1; }
		|	postfixExpr AUTOINC
    { $$ = MakeNtNode(T_postinc, "post++", $1, nil, nil); }
		|	postfixExpr AUTODEC
    { $$ = MakeNtNode(T_postdec, "post--", $1, nil, nil); }
		|	postfixExpr LSQR listExpr RSQR
    { $$ = MakeNtNode(T_index, "index", $1, $3, nil); }
		|	postfixExpr LPAR RPAR
    { $$ = MakeNtNode(T_call, "call", $1, nil, nil); }
		|	postfixExpr LPAR listExpr RPAR
    { $$ = MakeNtNode(T_call, "call", $1, $3, nil); }
		|	postfixExpr disarm DOT field_ID
    { ARM; $$ = MakeNtNode(T_dot, "dot", $1, $4, nil); }
		|	postfixExpr disarm ARROW field_ID
    { ARM; $$ = MakeNtNode(T_arrow, "arrow", $1, $4, nil); }
		;
/* precondition : armed.
 * postcondition : armed.
 */
primaryP1Expr	:	var_ID
    {
	/*
	 *	Although we are in armed state, as long as
	 *	all TYPE_IDs and IDs are disjoint, we have
	 *	no problem here.
	 */
	$$ = $1; 
    }
		|	SCALAR
    { $$ = MakeScalarNode(T_scalar); }
		|	LSCALAR
    { $$ = MakeScalarNode(T_lscalar); }
		|	REAL
    { $$ = MakeScalarNode(T_real); }
		|	CHAR_LIT
    { $$ = MakeScalarNode(T_char_lit); }
		|	STRING_LIT
    { $$ = MakeScalarNode(T_string_lit); }
		|	leftPAR listExpr RPAR
    { $$ = MakeNtNode(T_compound, "compoundExp", $2, nil, nil); }
		|	SIZEOF castHeadHead castHead arm RPAR
    { $$ = MakeNtNode(T_sizeof, "sizeof", $3, nil, nil); }
		;
leftPAR		:	LPAR
    { ARM; $$ = nil; }
		;
%%
yyerror() {
}
