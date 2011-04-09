#
# Makefile for C Minus
# Using flex and yacc
#

CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic 
OBJS = hash.o list.o y.tab.o lex.yy.o util.o main.o
INCL = ../include

cm: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) hlist.o -o ../cm -ly -ll

main.o: main.c $(INCL)
	$(CC) -c main.c -I$(INCL) $(CFLAGS)

list.o: list/
	cd list; make library

hash.o: hash/
	cd hash; make library

util.o: util.c $(INCL)
	$(CC) -c util.c -I$(INCL) $(CFLAGS)

lex.yy.c: tc.l
	flex tc.l

lex.yy.o: lex.yy.c y.tab.h $(INCL)
	$(CC) -c lex.yy.c -I$(INCL) $(CFLAGS)

y.tab.c: tc.y
	yacc -d tc.y

y.tab.o: y.tab.c $(INCL)
	$(CC) -c y.tab.c -I$(INCL) $(CFLAGS)

clean:
	rm -f $(OBJS)
	rm -f lex.yy.c y.tab.h y.tab.c
	cd stk; make clean
	cd list; make clean

cleanall: clean
	cd stk; make cleanall
	cd list; make cleanall
