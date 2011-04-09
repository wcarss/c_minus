/* Hash.h - Header and type definitions for Hash ADT */
/* Written by Wyatt Carss in January 2011 */
#ifndef _WHASH
#define _WHASH
#include "hlist/hlist.h"

#define _st(x) printf("Status is %d.\n", x);
#define HASH_STRING_LENGTH 64
#define HASH_SIZE 251

typedef struct _element{
	char id[HASH_STRING_LENGTH];
	char type[HASH_STRING_LENGTH];
	double value;
	int line_number;
	int column_number;
	struct _element *next;
}element;

typedef element * Element;

typedef struct _hash{
	HList *head;
	int size;
	int num_elements;
}hash;

typedef hash * Hash;

int index_access(char *key);
int hash_new(Hash *h);
int hash_kill(Hash *h);
int insert(Hash h, char *key, char *type, int lineno, int colno, int value);
HNode lookup(Hash h, char *key);
int remove_key(Hash h, char *key);
int is_empty(Hash h);
int hash_size(Hash h);
int print_hash(Hash h);

/* new_element(e, "eggs", "int", 5, 3, 1, NULL); */
int new_element(Element *e, char *id, char *type, double value, int line_number, int column_number, Element next);
int kill_element(char *str, Element *tail);
#endif
