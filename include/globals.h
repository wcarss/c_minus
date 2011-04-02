/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */
extern int lineno; /* source line number for listing */
extern int col; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {Stmt, Exp, Decl, List, Error} NodeKind;
typedef enum {If, Iter, Assign, Cmpd, Return, Call} StmtKind;
typedef enum {Factor, Const, Additive, Simple, Id, Term, Op} ExpKind;
typedef enum {Var, Fun, Param, Type} DeclKind;
typedef enum {Params, Local, StmtList, AdditiveList, TermList, Args} ListKind;

/* ExpType is used for type checking */
typedef enum {Dummy, Ummyd, Void,Integer,Array} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     int col;
     char *expected;
     int op;
     int val;
     char *name;
     char *type;
     NodeKind nodekind;
     /*ExpType type; *//* for type checking of exps */
     union { StmtKind stmt; ExpKind exp; DeclKind decl; ListKind list; } kind;
/*     union { int op;
             int val;
             char * name; } attr;
*/   } TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

#endif
