/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#endif

/* allocate global variables */
int lineno = 0;
int col = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int TraceScan = TRUE;
int TraceParse = TRUE; 
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int main( int argc, char * argv[] )
{ TreeNode * syntaxTree;
  char pgm[120]; /* source code file name */
  if (argc < 2)
    { fprintf(stderr,"usage: %s <filename>\n",argv[0]);
      exit(1);
    }
  if (argc == 3)
  {
    if(strcmp(argv[2], "-a") == 0)
    {
      TraceScan = FALSE;	
    }
  }
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  listing = stdout; /* send listing to screen */
  fprintf(listing,"\nC Minus compilation: %s\n",pgm);
#if NO_PARSE
  //if(a_flag == 0)
  while( (ttype=getToken())!= 0 )
    printToken( ttype, tokenString );
#else
  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing,"\nSyntax tree:\n");
    printTree(syntaxTree);
  }
#endif
  fclose(source);
  return 0;
}

