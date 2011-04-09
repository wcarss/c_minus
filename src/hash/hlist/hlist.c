/*typedef struct _hlist hlist;
typedef struct _hlist * HList;

typedef struct _hnode hnode;
typedef struct _hnode * HNode;

struct _hnode
{
  struct _hnode *next;
  struct _hnode *prev;
  void *v;
  int n;
  int id;
  char str[str_size];
  int (*free)(void *);
  int (*print)(void *);
};

struct _hlist
{
  struct hnode *root;
  struct hnode *current;
  int size;
  int id;
  char name[str_size];
};*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hlist.h"
#include "useful.h"

/* These are used to reset identifiers when no hnodes or hlists exist.
   They are not thread safe. It's just for convenience and pretty.
   See the note in hnode_kill for details */
int existing_hnodes = 0;
int existing_hlists = 0;

HNode hnode_new(char *key, char *type, int lineno, int colno, int value)
{
  static int i = 1;
  HNode new;

  if(NULL == (new = calloc(1, sizeof(struct _hnode))))
  {
    return NULL;
  }

  if(NULL == key || NULL == type) return NULL;

  if(existing_hnodes == 0) i = 1;

  memset(new->key, 0, str_size);
  memset(new->type, 0, str_size);
  new->id = i++;
  strcpy(new->type, type);
  strcpy(new->key, key);
  new->value = value;
  new->lineno = lineno;
  new->colno = colno;
  new->next = NULL;
  new->prev = NULL;
/*  new->next = next;
  new->prev = prev;
  new->v = v;
  new->free = free;
  new->print = _print;
  new->n = 0;*/

  existing_hnodes++;

  return new;
}


int hnode_kill(HNode n)
{
  int result = failure;

  if(NULL == n)
  {
    return got_null;
  }

  existing_hnodes--;
  /* This global variable resets the set of identifiers for hnodes.

    Every successful creation increases it, and every deletion
    decreases it. It's mostly to make results more intuitive when
    running a program that regularly makes and destroys hlists -
    at the point that no hnodes exist, id's start from one again. */
  /* This is not thread safe. */

  n->next = NULL;
  n->prev = NULL;
  memset(n->key, 0, str_size);
  memset(n->type, 0, str_size);

  free(n);

  return result;
}


HList hlist_new()
{
  static int i = 1;
  HList l;

  if(NULL == (l = calloc(1, sizeof(struct _hlist))))
  {
    return NULL;
  }

  /* See the note for existing hnodes in hnode_kill */
  if(existing_hlists == 0) i = 1;

  l->root = NULL;
  l->current = NULL;
  l->size = 0;
  l->id = i++;
  memset(l->name, 0, str_size);

  existing_hlists++;

  return l;
}


int hlist_kill(HList l)
{
  if(NULL == l)
  {
    return got_null;
  }

  /* See the note for existing hnodes in hnode_kill */
  existing_hlists--;

  l->current = l->root;
  while(got_null != hlist_remove(l));

  l->root = NULL;
  l->current = NULL;
  l->size = 0;
  l->id = 0;
  memset(l->name, 0, str_size);

  free(l);

  return success;
}


int hlist_add_front(HList l, HNode n)
{
  if(NULL == l || NULL == n)
  {
    return got_null;
  }

  n->next = l->root;
  /* If there are no elements in the hlist, root is null. */
  if(NULL != l->root)
    l->root->prev = n;
  l->root = n;

  /* make sure the hlist is in a good state for use */
  l->size += 1;
  if(1 == l->size)
    l->current = l->root;

  return success;
}


int hlist_add_back(HList l, HNode n)
{
  HNode temp = NULL;

  if(NULL == l || NULL == n)
  {
    return got_null;
  }

  if(NULL == l->root)
  {
    l->root = n;
    l->current = l->root;
  }
  else
  {
    /* save current hnode's position */
    temp = l->current;  
    l->current = l->root;

    /* find the end of the hlist */
    while(l->current->next != NULL)
    {
      l->current = l->current->next;
    }

    /* link n into the end */
    l->current->next = n;
    n->prev = l->current;

    /* restore current hnode */
    l->current = temp;
  }

  l->size += 1;

  return success;
}


int hlist_add_before(HList l, HNode n)
{
  if(NULL == l || NULL == n)
  {
    return got_null;
  }

  /* All of the hard edge cases are taken care of in front/back;
     this function will only ever add to the middle.
  */
  if(l->current == l->root)
  {
    hlist_add_front(l, n);
  }
  else
  { 
    n->next = l->current;
    n->prev = l->current->prev;
    n->prev->next = n;
    l->current->prev = n;
  }

  return success;
}


int hlist_add_after(HList l, HNode n)
{
  if(NULL == l || NULL == n)
  {
    return got_null;
  }

  /* All of the hardest edge cases are taken care of in front/back;
     this function will only ever add to the middle.
  */
  if(NULL == l->current || NULL == l->current->next)
  {
    hlist_add_back(l, n);
  }
  else
  {
    n->prev = l->current;
    n->next = l->current->next;
    n->next->prev = n;
    l->current->next = n;
  }

  return success;
}


int hlist_remove(HList l)
{
  HNode temp;

  if(NULL == l || NULL == l->root)
  {
    return got_null;
  }

  if(NULL == l->current)
  {
    return is_null;
  }

  if(l->current == l->root)
  {
    l->root = l->current->next;
    hnode_kill(l->current);
    l->current = l->root;

    /* if the root was alone, l->current is now NULL */
    if(NULL != l->current)
      l->current->prev = NULL;
  }
  else
  {
    temp = l->current;
/*  printf("l, l->current: %p, %p\n", l, l->current); fflush(stdout);
    printf("l->current->prev: %p\n", l->current->prev); fflush(stdout);
    printf("l->current->next: %p\n", l->current->next); fflush(stdout);
    printf("l->current->prev->next %p\n", l->current->prev->next); fflush(stdout);
*/  l->current->prev->next = l->current->next;
    l->current = l->current->prev;
    hnode_kill(temp);
    
    /* if we didn't just remove the tail, fix the next hnode's prev */
    if(l->current->next != NULL)
      l->current->next->prev = l->current;
  }

  l->size -= 1;

  return success;
}


int hlist_root(HList l)
{
  if(NULL == l || NULL ==  l->root)
  {
    return got_null;
  }

  l->current = l->root;

  return success;
}


int hlist_tail(HList l)
{
  if(NULL == l || NULL ==  l->root)
  {
    return got_null;
  }

  while(success == hlist_next(l));

  return success;
}


int hlist_next(HList l)
{
  int status = unset;

  if(NULL == l)
  {
    status = got_null;
  }

  if(unset == status && NULL == l->current)
  {
    status = is_null;
  }

  if(unset == status && NULL == l->current->next)
  {
    status = null_next;
  }

  if(unset == status || unset == null_next)
  {
    status = success;
    l->current = l->current->next;
  }

  return status;
}


int hlist_prev(HList l)
{
  int status = unset;

  if(NULL == l)
  {
    status = got_null;
  }

  if(unset == status && NULL == l->current)
  {
    status = is_null;
  }

  if(unset == status && NULL == l->current->prev)
  {
    status = null_next; /* null_next is equal to null_prev */
  }

  if(unset == status || null_next == status)
  {
    status = success;
    l->current = l->current->prev;
  }

  return status;
}


HNode hlist_get(HList l)
{
  if(NULL == l || NULL == l->current)
  {
    return NULL;
  }

  return l->current;
}

int hlist_print(HList l)
{
  HNode temp;
  printf("hlist has %d elements\n", l->size);

  if(NULL == l || NULL == l->current)
  {
    return got_null;
  }

  temp = l->current;
  l->current = l->root;
  while(l->current != NULL)
  //while(success == hlist_next(l))
  {
    printf("-%d-\n", l->current->id);
    l->current = l->current->next;
    /*printer = get_hnode(l);
    printer->print(printer);*/
  }
  l->current = temp;

  return success;
}
