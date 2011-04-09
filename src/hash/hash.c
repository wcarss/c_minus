/* Stack.h - Header and type definitions for Stack ADT */
/* Written by Wyatt Carss in January 2011 */

#include <stdlib.h>
#include<stdio.h>
#include<string.h>
#include "hlist/hlist.h"
#include "hash.h"
#include "hlist/useful.h"

#define _st(x) printf("Status is %d.\n", x);
#define HASH_STRING_LENGTH 64
/*
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
	Element *head;
	int size;
	int num_elements;
}hash;

typedef hash * Hash;
*/

int hash_new(Hash *h)
{
  int i = 0;
  *h = calloc(1,sizeof(hash));

  if(NULL == *h)
  {
    return 1; /*failure to allocate */
  }

  (*h)->size = HASH_SIZE;
  (*h)->num_elements = 0;
  (*h)->head = calloc(1,sizeof(Element)*HASH_SIZE);
  for(i = 0; i < HASH_SIZE; i++)
  {
    (*h)->head[i] = hlist_new();
  }

  return 0; /* success */
}


int hash_kill(Hash *h)
{
  int i;

  if(NULL == h || NULL == *h)
  {
    return -1;
  }

  for(i = 0; i < (*h)->size; i++)
  {
    hlist_kill((*h)->head[i]);
    (*h)->head[i] = NULL;
  }
  free((*h)->head);
  
  free(*h);

  return 0;
}

int index_access(char *key)
{
  int i;
  int index = 0;
  int len = strlen(key);

  for(i = 0; i < len; i++)
  {
    index = ((index << 1) + key[i]) % HASH_SIZE;
  }

  return index > 0 ? index : index * -1;
}

int insert(Hash h, char *key, char *type, int lineno, int colno, int value)
{
  int index = index_access(key);
  HList l = h->head[index];

  hlist_add_back(l, hnode_new(key, type, lineno, colno, value));

  return index;
}


HNode lookup(Hash h, char *key)
{
  int index = index_access(key);
  HList l = h->head[index];

  while(l->current != NULL)
  {
    if(strcmp(key, l->current->key) == 0)
    {
      return l->current;
    }
    l->current = l->current->next;
  }

  return NULL;
}


int remove_key(Hash h, char *key)
{
  int index = index_access(key);
  HList l = h->head[index];

  while(l->current != NULL)
  {
    if(strcmp(key, l->current->key) == 0)
    {
      hlist_remove(l);
    }
    l->current = l->current->next;
  }

  return 0;
}


int is_empty(Hash h)
{
  if(h->num_elements == 0)
  {
    return 1;
  }

  return 0;
}

int hash_size(Hash h)
{
  return h->size;
}

int print_hash(Hash h)
{
  HList l = NULL;
  HNode n = NULL;
  HNode temp = NULL;
  int i;

  printf("name\t\ttype\t\tline\t\tcol\t\tvalue\n");
  for(i = 0; i < HASH_SIZE; i++)
  {
    l = h->head[i];
    if(got_null == hlist_root(l))
    {
      continue;
    }

    temp = l->current;
    while(l->current != NULL)
    {
      n = l->current;
      if(strcmp(n->type, "undeclared") != 0)
        printf("%s\t\t%s\t\t%d\t\t%d\t\t%d\n", n->key, n->type, n->lineno, n->colno, n->value);
      l->current = l->current->next;
    }
    l->current = temp;
  }

  return 0;
}


/* new_element(e, "eggs", "int", 5, 3, 1, NULL) */
int new_element(Element *e, char *id, char *type, double value, int line_number, int column_number, Element next)
{
  return 0;
}


int kill_element(char *str, Element *tail)
{
  return 0;
}
