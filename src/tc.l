/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "y.tab.h"

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"else"          {return ELSE;}
"if"            {return IF;}
"int"           {return INT;}
"return"        {return RETURN;}
"void"		{return VOID;}
"while"		{return WHILE;}
"read"          {return READ;}
"write"         {return WRITE;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"<"             {return LT;}
"<="		{return LTEQ;}
">"		{return GT;}
">="		{return GTEQ;}
"=="             {return EQ;}
"!="             {return NEQ;}
"="            {return ASSIGN;}
";"             {return SEMICOLON;}
","		{return COMMA;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"["             {return LBRACKET;}
"]"             {return RBRACKET;}
"{"             {return LBRACE;}
"}"             {return RBRACE;}
{identifier}    {return ID;}
{number}        {return NUM;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"             { char c; int s = 0;
                  do
                  { c = input();
                    if (c == EOF) break;
                    if (c == '\n') lineno++;
		    
		    if (s == 0 && c == '*') s = 1;
		    else if (s == 1 && c == '*') s = 1;
		    else if (s == 1 && c == '/') s = 2;
		    else s = 0;
                  } while (s != 2);
                }
.               {return ERROR;}

%%

int getToken(void)
{ static int firstTime = TRUE;
  int currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString, yytext, MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
