#
# Makefile for List ADT
# Written by Wyatt Carss, April 2011
#
# 'make' to cleanly make the sample program cleanly
# 'make library' just to get list.o for linking
# 'make ugly' to get both
# 'make clean' to get rid of the library 
# 'make cleanall' to get rid of all compiled code
#

FLAGS = -g -Wall -pedantic --std=c99

default: library

fresh: main clean

library: 
	gcc -c hlist.c -o ../hlist.o $(FLAGS)

ugly: hlist.o main

main: hlist.o main.o
	gcc main.o hlist.o -o main $(FLAGS)

main.o: main.c
	gcc -c main.c -o main.o $(FLAGS)

hlist.o: hlist.c
	gcc -c hlist.c -o hlist.o $(FLAGS)

clean:
	rm -f *.o

cleanall: clean
	rm -f main
