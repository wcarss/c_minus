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

default: ugly

fresh: main clean

library: 
	gcc -c list.c -o ../list.o $(FLAGS)

ugly: list.o main

main: list.o main.o
	gcc main.o list.o -o main $(FLAGS)

main.o: main.c
	gcc -c main.c -o main.o $(FLAGS)

list.o: list.c
	gcc -c list.c -o list.o $(FLAGS)

clean:
	rm -f *.o

cleanall: clean
	rm -f main
