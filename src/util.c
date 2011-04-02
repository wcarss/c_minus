/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "y.tab.h"

/* prints a token and its lexeme to the listing file */
void printToken( int token, const char* tokenString )
{ 
    switch (token)
    { 
	case ELSE:
	case IF:
	case INT:
	case RETURN:
	case VOID:
	case WHILE:
	    //case READ:
	    //case WRITE:
	    fprintf(listing, "reserved word: %s\n", tokenString);
	    break;
	case PLUS: fprintf(listing,"+\n"); break;
	case MINUS: fprintf(listing,"-\n"); break;
	case TIMES: fprintf(listing,"*\n"); break;
	case OVER: fprintf(listing,"/\n"); break;
	case LT: fprintf(listing,"<\n"); break;
	case LTEQ: fprintf(listing,"<=\n"); break;
	case GT: fprintf(listing,">\n"); break;
	case GTEQ: fprintf(listing,">=\n"); break;
	case EQ: fprintf(listing,"==\n"); break;
	case NEQ: fprintf(listing,"!=\n"); break;
	case ASSIGN: fprintf(listing,"=\n"); break;
	case SEMICOLON: fprintf(listing,";\n"); break;
	case COMMA: fprintf(listing,",\n"); break;
	case LPAREN: fprintf(listing,"(\n"); break;
	case RPAREN: fprintf(listing,")\n"); break;
	case LBRACKET: fprintf(listing,"[\n"); break;
	case RBRACKET: fprintf(listing,"]\n"); break;
	case LBRACE: fprintf(listing,"{\n"); break;
	case RBRACE: fprintf(listing,"}\n"); break;
	case 0: fprintf(listing,"EOF\n"); break;
	case NUM: fprintf(listing, "NUM, val= %s\n", tokenString);
		  break;
	case ID: fprintf(listing, "ID, name= %s\n", tokenString);
		 break;
	case ERROR: fprintf(listing, "'%s'\n", tokenString);
		    break;
	default: /* should never happen */
		    fprintf(listing,"Unknown token: %d\n",token);
    }
}

/* creates a new statement node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
	fprintf(listing,"Out of memory error at line %d\n",lineno);
    else {
	for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = Stmt;
	t->kind.stmt = kind;
	t->lineno = lineno;
    }
    return t;
}

/* newExpNode creates a new expression node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
	fprintf(listing,"Out of memory error at line %d\n",lineno);
    else {
	for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = Exp;
	t->kind.exp = kind;
	t->lineno = lineno;
	t->type = Void;
    }
    return t;
}

/* newExpNode creates a new expression node for syntax tree construction
 */
TreeNode * newDeclNode(DeclKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
	fprintf(listing,"Out of memory error at line %d\n",lineno);
    else {
	for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = Decl;
	t->kind.decl = kind;
	t->lineno = lineno;
	t->type = Void;
    }
    return t;
}

/* newListNode creates a new List node for syntax tree construction
 */
TreeNode * newListNode(ListKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
	fprintf(listing,"Out of memory error at line %d\n",lineno);
    else {
	for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = List;
	t->kind.list = kind;
	t->lineno = lineno;
	t->type = Void;
    }
    return t;
}

/* newExpNode creates a new Error node for syntax tree construction
 */
TreeNode * newErrNode()
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
	fprintf(listing,"Out of memory error at line %d\n",lineno);
    else {
	for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
	t->sibling = NULL;
	t->nodekind = Error;
	t->lineno = lineno;
	t->col = col;
	t->type = Void;
    }
    return t;
}

/* allocates and makes a new copy of an existing string
 */
char * copyString(char * s)
{ int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    t = malloc(n);
    if (t==NULL)
	fprintf(listing,"Out of memory error at line %d\n",lineno);
    else strcpy(t,s);
    return t;
}

/* used by printTree to store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
    for (i=0;i<indentno;i++)
	fprintf(listing," ");
}

/* prints a syntax tree to the listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ 
    int i;
    INDENT;
    while (tree != NULL)
    {
	printSpaces();
	if (tree->nodekind==Stmt)
	{
	    switch (tree->kind.stmt)
	    {
		case If:
		    fprintf(listing,"If\n");
		    break;
		case Iter:
		    fprintf(listing,"While\n");
		    break;
		case Assign:
		    fprintf(listing,"Assignment:");
		    printToken(tree->attr.op,"\0");
		    break;
		case Cmpd:
		    fprintf(listing,"Compound\n");
		    break;
		case Return:
		    fprintf(listing,"Return\n");
		    break;
		case Call:
		    fprintf(listing,"Call of: %s\n",tree->attr.name);
		    break;
		default:
		    fprintf(listing,"Unknown ExpNode kind\n");
		    break;
	    }
	}
	else if (tree->nodekind==Exp)
	{ 
	    switch (tree->kind.exp)
	    {
		case Op:
		    fprintf(listing,"Op: ");
		    printToken(tree->attr.op,"\0");
		    break;
		case Const:
		    fprintf(listing,"Const: %d\n",tree->attr.val);
		    break;
		case Id:
		    fprintf(listing,"Id: %s\n",tree->attr.name);
		    break;
		case Factor:
		    fprintf(listing,"Factor\n");
		    break;
		case Additive:
		    fprintf(listing,"Additive\n");
		    break;
		case Simple:
		    fprintf(listing,"relop: ");
		    printToken(tree->attr.op,"\0");
		    break;
		case Term:
		    fprintf(listing,"Term\n");
		    break;
		default:
		    fprintf(listing,"Unknown ExpNode kind\n");
		    break;
	    }
	}
	else if(tree->nodekind == Decl)
	{
	    switch(tree->kind.decl)
	    {
		case Var:
		   fprintf(listing, "Var: %s\n", tree->attr.name);
		   break;	
		case Fun:	
		   fprintf(listing, "Fun: %s\n", tree->attr.name);
		   break;	
		case Param:	
		   fprintf(listing, "Param: %s\n", tree->attr.name);
		   break;	
		case Type:	
		   fprintf(listing, "Type: %s\n", tree->attr.name);
		   break;	
	    }
	}
	else if(tree->nodekind == List)
	{
	    switch(tree->kind.list)
	    {
		case Params:
		    fprintf(listing, "Param List\n");
		    break;
		case Local:
		    fprintf(listing, "Local Var List\n");
		    break;
		case StmtList:
		    fprintf(listing, "Stmt List\n");
		    break;
		case AdditiveList:
		    fprintf(listing, "Additive List\n");
		    break;
		case TermList:
		    fprintf(listing, "Term List\n");
		    break;
		case Args:
		    fprintf(listing, "Args List\n");
		    break;
	    }
	}
	else if(tree->nodekind == Error)
	{
	    int i;
	    fprintf(listing, "Error:\n%s\n", tree->attr.name);
	    for(i = 0; i < tree->col-1; i++)
	    {
	        printf("-");
	    }
	    printf("^\nExpected: %s\n", tree->expected);
	}
	else fprintf(listing,"Unknown node kind\n");
	for (i=0;i<MAXCHILDREN;i++)
	    printTree(tree->child[i]);
	tree = tree->sibling;
    }
    UNINDENT;
}
