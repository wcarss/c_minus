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

#define YYSTYPE type_t
static int savedLineNo;  /* ditto */
static int saved_col;
static TreeNode * savedTree; /* stores syntax tree for later return */

typedef union type_t type_t;

union type_t
{
	TreeNode *node;
	char *str;
	int op;
};

extern int yychar;

int yyerror(char * message)
{
  int i;

  fprintf(listing,"\nSyntax error at line %d: col %d, at ",lineno,col);
  printToken(yychar, next_token);
  fprintf(listing,"Current line:\n%s\n",current_line);

  for(i = 1; i < strlen(current_line); i++)
    fprintf(listing,"-");

  fprintf(listing,"^\n");
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with the C Minus scanner
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

%% 

program     : declaration_list
                 { savedTree = $1.node; } 
            ;

declaration_list 	: declaration_list declaration
			{
			    TreeNode * t = $1.node;
			    if (t != NULL)
			    {
				while (t->sibling != NULL)
				    t = t->sibling;
				t->sibling = $2.node;
				$$.node = $1.node;
			    }
			    else $$.node = $2.node;
			}
			| declaration
			    { $$.node = $1.node; }
			;

declaration	: var_declaration
			{ $$.node = $1.node; }
		| fun_declaration
			{ $$.node = $1.node; }
		;

var_declaration	: type_specifier ID SEMICOLON
			{
			  $$.node = newDeclNode(Var);
			  $$.node->type = $1.str;
			  $$.node->name = copyString(tokenString);
			}
		| type_specifier ID
			{
			  savedLineNo = lineno; 
			  $$.str = copyString(tokenString);
			}
		LBRACKET NUM RBRACKET SEMICOLON
			{
			  $$.node = newDeclNode(Var);
			  $$.node->type= copyString("Array");
			  $$.node->name = $3.str;
			  $$.node->lineno = savedLineNo;
			  $$.node->val = atoi(tokenString); /* $5.op; */
			}
		;

type_specifier	: INT
			{
			  $$.str = copyString("Integer");
			}
		| VOID
			{
			  $$.str = copyString("Void");
			}
		;

fun_declaration	: type_specifier ID 
			{
			  savedLineNo = lineno;
			  $$.str = copyString(tokenString);
			}
			LPAREN params RPAREN compound_stmt
			{
			  $$.node = newDeclNode(Fun);
			  $$.node->name = $3.str;
			  $$.node->lineno = savedLineNo;
			  $$.node->type = $1.str;
			  $$.node->child[0] = $5.node;
			  $$.node->child[1] = $7.node;
			}
		;

params	: param_list
		{ $$.node = $1.node; }
	| VOID
	;

param_list	: param_list COMMA param
			{
			  TreeNode * t = $1.node;
			  if (t != NULL)
			  {
				while (t->sibling != NULL)
				t = t->sibling;
				t->sibling = $3.node;
				$$.node = $1.node;
			  }
			  else $$.node = $3.node;
			}
		| param
			{ $$.node = $1.node; }
		;

param	: type_specifier ID
		{
		  $$.node = newDeclNode(Param);
		  $$.node->type = $1.str;
		  $$.node->name = copyString(tokenString);
		}
	| type_specifier ID
		{
		  savedLineNo = lineno;
		  $$.str = copyString(tokenString);
		}
			LBRACKET RBRACKET
		{
		  $$.node = newDeclNode(Param);
		  $$.node->type = $1.str;
		  $$.node->name = $3.str;
		  $$.node->lineno = savedLineNo;
		}
	;

compound_stmt	: LBRACE local_declarations stmt_list RBRACE
			{
			  $$.node = newStmtNode(Cmpd);
			  $$.node->child[0] = $2.node;
			  $$.node->child[1] = $3.node;
			}
		;

local_declarations	: local_declarations var_declaration
				{
				  TreeNode * t = $1.node;
				/*printf("t is %p\n", t); fflush(stdout);*/
				  if (t != NULL)
				  {
					while (t->sibling != NULL)
					{
						t = t->sibling;
					}
					t->sibling = $2.node;
					$$.node = $1.node;
				  }
				  else
				  {
					 $$.node = $2.node;
					 if($2.node == 0x3) printf(".. so it is here\n");
				  }
				}
			| { $$.node = NULL; }
			;

stmt_list	: stmt_list stmt
			{
			  TreeNode * t = $1.node;
			  if (t != NULL)
			  {
				while (t->sibling != NULL)
				t = t->sibling;
				t->sibling = $2.node;
				$$.node = $1.node;
			  }
			  else $$.node = $2.node;
			}
		| { $$.node = NULL; }
		;

stmt	: expr_stmt
		{ saved_col = col; $$.node = $1.node; }
	| compound_stmt
		{ saved_col = col; $$.node = $1.node; }
	| selection_stmt
		{ saved_col = col; $$.node = $1.node; }
	| iteration_stmt
		{ saved_col = col; $$.node = $1.node; }
	| return_stmt
		{ saved_col = col; $$.node = $1.node; }
	;

expr_stmt	: expr SEMICOLON
			{ $$.node = $1.node; }
		| SEMICOLON
		;

selection_stmt	: IF LPAREN expr RPAREN stmt
			{
			  $$.node = newStmtNode(If);
			  $$.node->child[0] = $3.node;
			  $$.node->child[1] = $5.node;
			}
		| IF LPAREN expr RPAREN stmt ELSE stmt
			{
			  $$.node = newStmtNode(If);
			  $$.node->child[0] = $3.node;
			  $$.node->child[1] = $5.node;
			  $$.node->child[2] = $7.node;
			}
		;

iteration_stmt	: WHILE LBRACE expr RBRACE stmt
			{
			  $$.node = newStmtNode(Iter);
			  $$.node->child[0] = $3.node;
			  $$.node->child[1] = $5.node;
			}
		;

return_stmt	: RETURN SEMICOLON
			{
			  $$.node = newStmtNode(Return);
			  $$.node->child[0] = NULL;
			}
		| RETURN expr SEMICOLON
			{
			  $$.node = newStmtNode(Return);
			  $$.node->child[0] = $2.node;
			}
		;

expr	: var ASSIGN expr
		{
		  $$.node = newStmtNode(Assign);
		  $$.node->child[0] = $1.node;
		  $$.node->op = ASSIGN; 
		  $$.node->child[1] = $3.node;
		}
	| simple_expr
		{
		  $$.node = $1.node;
		}
	;

var	: ID
		{
		  $$.node = newExpNode(Id);
		  $$.node->name = copyString(tokenString);
		}
	| ID
		{
		  savedLineNo = lineno;
		  $$.str = copyString(tokenString);
		}
		LBRACKET expr RBRACKET
		{
		  $$.node = newExpNode(Id);
		  $$.node->name = $2.str;
		  $$.node->lineno = savedLineNo;
		  $$.node->child[1] = $4.node;
		}
	;

simple_expr	: additive_expr relop additive_expr
			{
			  $$.node = newExpNode(Op);
			  $$.node->child[0] = $1.node;
			  $$.node->op = $2.op;
			  $$.node->child[2] = $3.node;
			}
		| additive_expr
			{ $$.node = $1.node; }
		| additive_expr error
			{
			  $$.node = newErrNode();
			  $$.node->name = copyString(current_line);
			  $$.node->expected = copyString("Relation or Additive expression (eg '1 != 0' or '4 + 6')");
			  $$.node->col = col;
			}
		;

relop : LTEQ
		{
		  $$.op = LTEQ;
		}
	| LT
		{ 
		  $$.op = LT;
		}
	| GT	
		{ 
		  $$.op = GT;
		}
	| GTEQ	
		{ 
		  $$.op = GTEQ;
		}
	| EQ	
		{ 
		  $$.op = EQ;
		}
	| NEQ	
		{ 
		  $$.op = NEQ;
		}
	;

additive_expr	: additive_expr addop term
			{
				$$.node = newExpNode(Op);
				$$.node->child[0] = $1.node;
				$$.node->child[1] = $3.node;
				$$.node->op = $2.op;
			}
		| term
			{ $$.node = $1.node; }
		| error
			{
		  $$.node = newErrNode();
		  $$.node->name = copyString(current_line);
		  $$.node->expected = copyString("Term (eg '5' or var_name)");
		  $$.node->col =  col;
			}
		;

addop	: PLUS
		{
		  $$.op = PLUS;
		}
	| MINUS
		{
		  $$.op = MINUS;
		}
	;

term	: term mulop factor
		{
			$$.node = newExpNode(Op);
				$$.node->child[0] = $1.node;
				$$.node->child[1] = $3.node;
				$$.node->op = $2.op;
		}
	| factor
		{ $$.node = $1.node; }
	;

mulop	: TIMES
		{ 
		  $$.op = TIMES;
		}
	| OVER
		{
		  $$.op = OVER;
		}
	;

factor	: LPAREN expr RPAREN
		{
		  /*$$.node = newExpNode(Factor);
		  $$.node->child[0] = $2.node;*/
		  $$.node = $2.node;
		}
	| var
		{ $$.node = $1.node; }
	| call
		{ $$.node = $1.node; }
	| NUM
		{ 
		  $$.node = newExpNode(Const);
		  $$.node->val = atoi(next_token);
		}
	;

call	: ID
		{
		  savedLineNo = lineno;
		  $$.str = copyString(tokenString);
		}
		LPAREN args RPAREN
		{
		  $$.node = newStmtNode(Call);
		  $$.node->name = $2.str;
		  $$.node->child[0] = $4.node;
		}
	;

args	: arg_list
		{ $$.node = $1.node; }
	| { $$.node = NULL; }
	;

arg_list	: arg_list COMMA expr
			{
			  TreeNode * t = $1.node;
        	          if (t != NULL)
        	          {
				while (t->sibling != NULL)
				t = t->sibling;
				t->sibling = $3.node;
				$$.node = $1.node;
			  }
			  else $$.node = $3.node;
			}
		| expr
			{ $$.node = $1.node; }
		;
