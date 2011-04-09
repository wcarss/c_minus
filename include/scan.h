/****************************************************/
/* File: scan.h                                     */
/* The scanner interface for the TINY compiler      */
/****************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_

#include "../src/list/list.h"
#include "../src/hash/hash.h"

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* tokenString array stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN+1];
extern char next_token[MAXTOKENLEN+1];
extern char current_line[100];
extern List l;
extern Hash h;

/* returns the next token in source file
 */
int getToken(void);

#endif
