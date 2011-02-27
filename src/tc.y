/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */

extern int yychar;

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d, col %d: %s\n",lineno,col,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void) { 
  yyparse();
  return savedTree;
}

%}

%token ELSE IF INT RETURN VOID WHILE
%token ID NUM 
%token PLUS MINUS TIMES OVER LT LTEQ GT GTEQ EQ NEQ ASSIGN SEMICOLON COMMA LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token ERROR 

%nonassoc LT LTEQ GT GTEQ EQ NEQ
%left PLUS MINUS
%left TIMES OVER

%% /* Grammar for TINY */

program     : declaration_list
                 { savedTree = $1;} 
            ;

declaration_list 	: declaration_list declaration 
			| declaration
			;

declaration	: var_declaration
		| fun_declaration
		;

var_declaration	: type_specifier ID SEMICOLON
		| type_specifier ID LBRACKET NUM RBRACKET SEMICOLON
		;

type_specifier	: INT
		| VOID
		;

fun_declaration	: type_specifier ID LPAREN params RPAREN compount_stmt
		;

params	: param_list
	| VOID
	;

param_list	: param_list COMMA param
		| param
		;

param	: type_specifier ID
	| type_specifier ID LBRACKET RBRACKET
	;

compount_stmt	: LBRACE local_declarations stmt_list RBRACE
		;

local_declarations	: local_declarations var_declaration
			| /*empty*/
			;

stmt_list	: stmt_list stmt
		| /*empty*/
		;

stmt	: expr_stmt
	| compount_stmt
	| selection_stmt
	| iteration_stmt
	| return_stmt
	;

expr_stmt	: expr SEMICOLON
		| SEMICOLON
		;

selection_stmt	: IF LPAREN expr RPAREN stmt
		| IF LPAREN expr RPAREN stmt ELSE stmt
		;

iteration_stmt	: WHILE LBRACE expr RBRACE stmt
		;

return_stmt	: RETURN SEMICOLON
		| RETURN expr SEMICOLON
		;

expr	: var ASSIGN expr
	| simple_expr
	;

var	: ID
	| ID LBRACKET expr RBRACKET
	;

simple_expr	: additive_expr relop additive_expr
		| additive_expr
		;

relop	: LTEQ
	| LT
	| GT
	| GTEQ
	| EQ
	| NEQ
	;

additive_expr	: additive_expr addop term
		| term
		;

addop	: PLUS
	| MINUS
	;

term	: term mulop factor
	| factor
	;

mulop	: TIMES
	| OVER
	;

factor	: LPAREN expr RPAREN
	| var
	| call
	| NUM
	;

call	: ID LPAREN args RPAREN
	;

args	: arg_list
	| /*empty*/
	;

arg_list	: arg_list COMMA expr
		| expr


/*
stmt_seq    : stmt_seq SEMICOLON stmt
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; }
                     else $$ = $3;
                 }
            | stmt  { $$ = $1; }
            ;

stmt        : if_stmt { $$ = $1; }
            | repeat_stmt { $$ = $1; }
            | assign_stmt { $$ = $1; }
            | read_stmt { $$ = $1; }
            | write_stmt { $$ = $1; }
            | error  { $$ = NULL; }
            ;

if_stmt     : IF exp stmt_seq
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $2;
                   $$->child[1] = $3;
                 }
            | IF exp stmt_seq ELSE stmt_seq
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $2;
                   $$->child[1] = $3;
                   $$->child[2] = $5;
                 }
            ;

repeat_stmt : WHILE stmt_seq exp
                 { $$ = newStmtNode(RepeatK);
                   $$->child[0] = $2;
                   $$->child[1] = $3;
                 }
            ;

assign_stmt : ID { savedName = copyString(tokenString);
                   savedLineNo = lineno; }
              ASSIGN exp
                 { $$ = newStmtNode(AssignK);
                   $$->child[0] = $4;
                   $$->attr.name = savedName;
                   $$->lineno = savedLineNo;
                 }
            ;

read_stmt   : READ ID
                 { $$ = newStmtNode(ReadK);
                   $$->attr.name = copyString(tokenString);
                 }
            ;

write_stmt  : WRITE exp
                 { $$ = newStmtNode(WriteK);
                   $$->child[0] = $2;
                 }
            ;

exp         : exp LT exp 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = LT;
                 }
            | exp GT exp 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = GT;
                 }
            | exp EQ exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = EQ;
                 }
            | exp PLUS exp 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = PLUS;
                 }
            | exp MINUS exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MINUS;
                 } 
            | exp TIMES exp 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = TIMES;
                 }
            | exp OVER exp
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = OVER;
                 }
            | LPAREN exp RPAREN
                 { $$ = $2; }
            | NUM
                 { $$ = newExpNode(ConstK);
                   $$->attr.val = atoi(tokenString);
                 }
            | ID { $$ = newExpNode(IdK);
                   $$->attr.name =
                         copyString(tokenString);
                 }
            | error { $$ = NULL; }
            ;
*/
