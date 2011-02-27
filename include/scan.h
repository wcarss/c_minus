/****************************************************/
/* File: scan.h                                     */
/* The scanner interface for the TINY compiler      */
/****************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* tokenString array stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN+1];

/* returns the next token in source file
 */
int getToken(void);

#endif
