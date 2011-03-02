#
# Makefile for TINY
# Using flex and yacc
#

CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic 
OBJS = y.tab.o lex.yy.o util.o main.o
INCLUDES = -I../include
I=../include

tc: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ../tc -ly -ll

main.o: main.c $(I)/globals.h $(I)/util.h $(I)/scan.h $(I)/parse.h
	$(CC) -c main.c $(INCLUDES) $(CFLAGS)

util.o: util.c $(I)/util.h $(I)/globals.h y.tab.h
	$(CC) -c util.c $(INCLUDES) $(CFLAGS)

lex.yy.c: tc.l
	flex tc.l

lex.yy.o: lex.yy.c $(I)/globals.h $(I)/util.h y.tab.h
	$(CC) -c lex.yy.c $(INCLUDES) $(CFLAGS)

y.tab.c: tc.y
	yacc -d tc.y

y.tab.o: y.tab.c $(I)/globals.h $(I)/util.h $(I)/scan.h $(I)/parse.h
	$(CC) -c y.tab.c $(INCLUDES) $(CFLAGS)

clean:
	rm -f $(OBJS)
	rm -f lex.yy.c y.tab.h y.tab.c
	cd stk; make clean

cleanall: clean
	cd stk; make cleanall
