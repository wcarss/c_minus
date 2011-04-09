#
# Makefile for Stack ADT
# Written by Wyatt Carss, January 2011
#
# 'make' to cleanly make the sample program cleanly
# 'make library' just to get hash.o for linking
# 'make ugly' to get both
# 'make clean' to get rid of the library 
# 'make cleanall' to get rid of all compiled code
#

FLAGS = -g -Wall -pedantic --std=c99

default: library

fresh: main clean

library: hlist.o hash.o clean

ugly: main

main: hlist.o main.o hash.o
	gcc main.o hash.o hlist.o -o main $(FLAGS)

hlist.o: 
	cd hlist; make
	cp hlist.o ../

main.o: main.c
	gcc -c main.c -o main.o $(FLAGS)

hash.o: hash.c
	gcc -c hash.c -o ../hash.o $(FLAGS)

clean:
	rm -f *.o

cleanall: clean
	rm -f main
