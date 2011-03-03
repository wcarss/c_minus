/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "y.tab.h"

/* may be of use later
"input"          {col += 5; return READ;}
"output"         {col += 6; return WRITE;}
*/

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
char next_token[MAXTOKENLEN+1];
char current_line[100];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"else"          {col += 4; return ELSE;}
"if"            {col += 2; return IF;}
"int"           {col += 3; return INT;}
"return"        {col += 6; return RETURN;}
"void"		{col += 4; return VOID;}
"while"		{col += 5; return WHILE;}
"+"             {col += 1; return PLUS;}
"-"             {col += 1; return MINUS;}
"*"             {col += 1; return TIMES;}
"/"             {col += 1; return OVER;}
"<"             {col += 1; return LT;}
"<="		{col += 2; return LTEQ;}
">"		{col += 1; return GT;}
">="		{col += 2; return GTEQ;}
"=="             {col += 2; return EQ;}
"!="             {col += 2; return NEQ;}
"="            {col += 1; return ASSIGN;}
";"             {col += 1; return SEMICOLON;}
","		{col += 1; return COMMA;}
"("             {col += 1; return LPAREN;}
")"             {col += 1; return RPAREN;}
"["             {col += 1; return LBRACKET;}
"]"             {col += 1; return RBRACKET;}
"{"             {col += 1; return LBRACE;}
"}"             {col += 1; return RBRACE;}
{identifier}    {col += strlen(yytext); return ID;}
{number}        {col += strlen(yytext); return NUM;}
{newline}       {col = 0; memset(current_line, 0, 100); lineno++;}
{whitespace}    {col += strlen(yytext); strcat(current_line, yytext);/* skip whitespace */}
"/*"             { char c; int s = 0;
                  do
                  { c = input();
		    col++;
                    if (c == EOF) break;
                    if (c == '\n'){col = 0; memset(current_line, 0, 100); lineno++;}
		    
		    if (s == 0 && c == '*') s = 1;
		    else if (s == 1 && c == '*') s = 1;
		    else if (s == 1 && c == '/') s = 2;
		    else s = 0;
                  } while (s != 2);
                }
.               {col += strlen(yytext); return ERROR;}

%%

int getToken(void)
{ static int firstTime = TRUE;
  int currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
    strcpy(next_token, "first");
  }
  currentToken = yylex();
  strcat(current_line, yytext);
  if(strcmp(next_token, "first") != 0)
  {
    strncpy(tokenString, next_token, MAXTOKENLEN);
  }
  /*if(currentToken == NUM || currentToken == ID)
  */  strncpy(next_token, yytext, MAXTOKENLEN);
  if (TraceScan == TRUE) {
    fprintf(listing,"\t%d:%d: ",lineno,col);
    printToken(currentToken,next_token);
  }
  return currentToken;
}
