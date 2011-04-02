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

%% /* Grammar) for C Minus */

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

var_declaration	: type_specifier ID
			{
			  $$.node = newDeclNode(Var);
			  $$.node->child[0] = $1.node;
			  $$.node->attr.name = copyString(tokenString);
			}
		  SEMICOLON
/* changed 'NUM' to 'simple expr #GRAMMAR CHANGE */
		| type_specifier ID
			{
			  savedLineNo = lineno; 
			  $$.str = copyString(tokenString);
			/*copyString(tokenString);*/
			}
			LBRACKET simple_expr RBRACKET SEMICOLON
			{
			  $$.node = newDeclNode(Var);
			  $$.node->attr.name = $3.str;
			  $$.node->lineno = savedLineNo;
			  $$.node->child[0] = $1.node;
			  $$.node->child[1] = $5.node;
			}
		| type_specifier ID
			{
			  $$.node = NULL;
			}
			LBRACKET error
			{
			  $$.node = newErrNode();
			  $$.node->attr.name = copyString(current_line);
			  $$.node->expected = copyString("size of array (eg '[1]' or '[SIZE]')");
			  $$.node->col = col;
			}
		;

type_specifier	: INT
			{
			  $$.node = newDeclNode(Type);
			  $$.node->attr.name = copyString("Integer");
			  $$.node->type = Integer;
			}
		| VOID
			{
			  $$.node = newDeclNode(Type);
			  $$.node->attr.name = copyString("Void");
			  $$.node->type = Void;
			}
		| error
			{
			  $$.node = newErrNode();
			  $$.node->attr.name = copyString(current_line);
			  $$.node->expected = copyString("INT or VOID");
			  $$.node->col = col;
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
			  $$.node->attr.name = $3.str;
			  $$.node->lineno = savedLineNo;
			  $$.node->child[0] = $1.node;
			  $$.node->child[1] = $5.node;
			  $$.node->child[2] = $7.node;
			}
		;

params	: param_list
		{ $$.node = $1.node; }
	| VOID
	;

param_list	: param_list COMMA param
			{
			  /*$$.node = newListNode(Param);
			  $$.node->child[0] = $1.node;
			  $$.node->child[1] = $3.node;*/
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
		  $$.node->child[0] = $1.node;
		  $$.node->attr.name = copyString(tokenString);
		}
	| type_specifier ID
		{
		  savedLineNo = lineno;
		  $$.str = copyString(tokenString);
		}
			LBRACKET RBRACKET
		{
		  $$.node = newDeclNode(Param);
		  $$.node->child[0] = $1.node;
		  $$.node->attr.name = $3.str;
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
				  if (t != NULL)
				  {
					while (t->sibling != NULL)
					t = t->sibling;
					t->sibling = $2.node;
					$$.node = $1.node;
				  }
				  else $$.node = $2.node;	/*
				  $$.node = newListNode(Local);
				  $$.node->child[0] = $1.node;
				  $$.node->child[1] = $2.node;*/
				}
			| { $$.node = NULL; }/*empty*/
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
			  else $$.node = $2.node;/*
			  $$.node = newListNode(StmtList);
			  $$.node->child[0] = $1.node;
			  $$.node->child[1] = $2.node;*/
			}
		| { $$.node = NULL; }/*empty*/
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
		| RETURN error
			{
			  $$.node = newErrNode();
			  $$.node->attr.name = copyString(current_line);
			  $$.node->expected = copyString("semicolon");
			  $$.node->col =  col;	
			} 
		;

expr	: var ASSIGN expr
		{
		  $$.node = newStmtNode(Assign);
		  $$.node->child[0] = $1.node;
		  $$.node->attr.op = ASSIGN; 
		  $$.node->child[1] = $3.node;
		}
	| simple_expr
		{
		  $$.node = $1.node;
		}
/*	| error { 
		  $$.node = newErrNode();
		  $$.node->attr.name = copyString(current_line);
		  $$.node->expected = copyString("expression");
		  $$.node->col =  col;printf("\n\nunexpected!\n\n");
		}*/
	;

var	: ID
		{
		  $$.node = newExpNode(Id);
		  $$.node->attr.name = copyString(tokenString);
		}
	| ID
		{
		  savedLineNo = lineno;
		  $$.str = copyString(tokenString);
		}
		LBRACKET expr RBRACKET
		{
		  $$.node = newExpNode(Id);
		  $$.node->attr.name = $2.str;
		  $$.node->lineno = savedLineNo;
		  $$.node->child[1] = $4.node;
		}
	;

simple_expr	: additive_expr relop additive_expr
			{
			  $$.node = newExpNode(Simple);
			  $$.node->child[0] = $1.node;
			  /*$$.node->child[1] = $2.node;*/
			  $$.node->attr.op = $2.op;
			  $$.node->child[2] = $3.node;
			}
		| additive_expr
			{ $$.node = $1.node; }
		| additive_expr error
			{
			  $$.node = newErrNode();
			  $$.node->attr.name = copyString(current_line);
			  $$.node->expected = copyString("Relation or Additive expression (eg '1 != 0' or '4 + 6')");
			  $$.node->col = col;
			}
		;

/*relop	: LTEQ
		{
		  $$.node = newExpNode(Op);
		  $$.node->attr.op = LTEQ;
		}
	| LT
		{ 
		  $$.node = newExpNode(Op);
		  $$.node->attr.op = LT;
		}
	| GT	
		{ 
		  $$.node = newExpNode(Op);
		  $$.node->attr.op = GT;
		}
	| GTEQ	
		{ 
		  $$.node = newExpNode(Op);
		  $$.node->attr.op = GTEQ;
		}
	| EQ	
		{ 
		  $$.node = newExpNode(Op);
		  $$.node->attr.op = EQ;
		}
	| NEQ	
		{ 
		  $$.node = newExpNode(Op);
		  $$.node->attr.op = NEQ;
		}
	| error
		{
		  $$.node = newErrNode();
		  $$.node->attr.name = copyString(current_line);
		  $$.node->expected = copyString("Operator (eg <, >, <=, ==)");
		  $$.node->col = col;
		  /*yyerrok;*/ 
	/*	}
	;
*/

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
		/*	  TreeNode * t = $1.node;
        	          if (t != NULL)
        	          {
				while (t->sibling != NULL)
					t = t->sibling;
				/*t->sibling = $3.node;
				t->child[0] = $2.node;*/
				$$.node = newExpNode(Simple);
				$$.node->child[0] = $1.node;
				$$.node->child[1] = $3.node;
				$$.node->attr.op = $2.op;
				/*$$.node = $1.node;*/
		/*	  }
			  else $$.node = $3.node;/*
			  $$.node = newListNode(AdditiveList);
			  $$.node->child[0] = $1.node;
			  $$.node->child[1] = $2.node;
			  $$.node->child[2] = $3.node;*/
			}
		| term
			{ $$.node = $1.node; }
		| error
			{
		  $$.node = newErrNode();
		  $$.node->attr.name = copyString(current_line);
		  $$.node->expected = copyString("Term (eg '5' or var_name)");
		  $$.node->col =  col;/*printf("\n\nunexpected!\n\n");*/
			}
		;

addop	: PLUS
		{
		  /*$$.node = newExpNode(Op);
		  $$.node->attr.op = PLUS;*/
		  $$.op = PLUS;
		}
	| MINUS
		{
		  /*$$.node = newExpNode(Op);
		  $$.node->attr.op = MINUS; */
		  $$.op = MINUS;
		}
	;

term	: term mulop factor
		{
		/*	TreeNode * t = $1.node;
        	          if (t != NULL)
        	          {
				while (t->sibling != NULL)
					t = t->sibling;
				/*t->sibling = $3.node;
				t->child[0] = $2.node;*/
			$$.node = newExpNode(Simple);
				$$.node->child[0] = $1.node;
				$$.node->child[1] = $3.node;
				$$.node->attr.op = $2.op;
		/*	  }
			  else $$.node = $3.node;/*
			  $$.node = newListNode(TermList);
			  $$.node->child[0] = $1.node;
			  $$.node->child[1] = $2.node;
			  $$.node->child[2] = $3.node;*/
		}
	| factor
		{ $$.node = $1.node; }
	;

mulop	: TIMES
		{ 
		  /*$$.node = newExpNode(Op);
		  $$.node->attr.op = TIMES; */
		  $$.op = TIMES;
		}
	| OVER
		{
		  /*$$.node = newExpNode(Op); 
		  $$.node->attr.op = OVER; */
		  $$.op = OVER;
		}
	;

factor	: LPAREN expr RPAREN
		{
		  $$.node = newExpNode(Factor);
		  $$.node->child[0] = $2.node;
		}
	| var
		{ $$.node = $1.node; }
	| call
		{ $$.node = $1.node; }
	| NUM
		{ 
		  $$.node = newExpNode(Const);
		  printf("the token is a NUM, and it's -%s-\n", next_token);
		  $$.node->attr.val = atoi(next_token);
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
		  $$.node->attr.name = $2.str;
		  $$.node->child[0] = $4.node;
		}
	;

args	: arg_list
		{ $$.node = $1.node; }
	| { $$.node = NULL; }/*empty*/
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
			  else $$.node = $3.node;/*
			  $$.node = newListNode(Args);
			  $$.node->child[0] = $1.node;
			  $$.node->child[1] = $2.node;*/
			}
		| expr
			{ $$.node = $1.node; }
		;


/*
stmt_seq    : stmt_seq SEMICOLON stmt
                 { TreeNode * t = $1.node;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3.node;
                     $$.node = $1.node; }
                     else $$.node = $3.node;
                 }
            | stmt  { $$.node = $1.node; }
            ;

stmt        : if_stmt { $$.node = $1.node; }
            | repeat_stmt { $$.node = $1.node; }
            | assign_stmt { $$.node = $1.node; }
            | read_stmt { $$.node = $1.node; }
            | write_stmt { $$.node = $1.node; }
            | error  { $$.node = NULL; }
            ;

if_stmt     : IF exp stmt_seq
                 { $$.node = newStmtNode(IfK);
                   $$.node->child[0] = $2.node;
                   $$.node->child[1] = $3.node;
                 }
            | IF exp stmt_seq ELSE stmt_seq
                 { $$.node = newStmtNode(IfK);
                   $$.node->child[0] = $2.node;
                   $$.node->child[1] = $3.node;
                   $$.node->child[2] = $5.node;
                 }
            ;

repeat_stmt : WHILE stmt_seq exp
                 { $$.node = newStmtNode(RepeatK);
                   $$.node->child[0] = $2.node;
                   $$.node->child[1] = $3.node;
                 }
            ;

assign_stmt : ID { savedName = copyString(tokenString);
                   savedLineNo = lineno; }
              ASSIGN exp
                 { $$.node = newStmtNode(AssignK);
                   $$.node->child[0] = $4.node;
                   $$.node->attr.name = savedName;
                   $$.node->lineno = savedLineNo;
                 }
            ;

read_stmt   : READ ID
                 { $$.node = newStmtNode(ReadK);
                   $$.node->attr.name = copyString(tokenString);
                 }
            ;

write_stmt  : WRITE exp
                 { $$.node = newStmtNode(WriteK);
                   $$.node->child[0] = $2.node;
                 }
            ;

exp         : exp LT exp 
                 { $$.node = newExpNode(OpK);
                   $$.node->child[0] = $1.node;
                   $$.node->child[1] = $3.node;
                   $$.node->attr.op = LT;
                 }
            | exp GT exp 
                 { $$.node = newExpNode(OpK);
                   $$.node->child[0] = $1.node;
                   $$.node->child[1] = $3.node;
                   $$.node->attr.op = GT;
                 }
            | exp EQ exp
                 { $$.node = newExpNode(OpK);
                   $$.node->child[0] = $1.node;
                   $$.node->child[1] = $3.node;
                   $$.node->attr.op = EQ;
                 }
            | exp PLUS exp 
                 { $$.node = newExpNode(OpK);
                   $$.node->child[0] = $1.node;
                   $$.node->child[1] = $3.node;
                   $$.node->attr.op = PLUS;
                 }
            | exp MINUS exp
                 { $$.node = newExpNode(OpK);
                   $$.node->child[0] = $1.node;
                   $$.node->child[1] = $3.node;
                   $$.node->attr.op = MINUS;
                 } 
            | exp TIMES exp 
                 { $$.node = newExpNode(OpK);
                   $$.node->child[0] = $1.node;
                   $$.node->child[1] = $3.node;
                   $$.node->attr.op = TIMES;
                 }
            | exp OVER exp
                 { $$.node = newExpNode(OpK);
                   $$.node->child[0] = $1.node;
                   $$.node->child[1] = $3.node;
                   $$.node->attr.op = OVER;
                 }
            | LPAREN exp RPAREN
                 { $$.node = $2.node; }
            | NUM
                 { $$.node = newExpNode(ConstK);
                   $$.node->attr.val = atoi(tokenString);
                 }
            | ID { $$.node = newExpNode(IdK);
                   $$.node->attr.name =
                         copyString(tokenString);
                 }
            | error { $$.node = NULL; }
            ;
*/
