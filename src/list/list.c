/*typedef struct _list list;
typedef struct _list * List;

typedef struct _node node;
typedef struct _node * Node;

struct _node
{
  struct _node *next;
  struct _node *prev;
  void *v;
  int n;
  int id;
  char str[str_size];
  int (*free)(void *);
  int (*print)(void *);
};

struct _list
{
  struct node *root;
  struct node *current;
  int size;
  int id;
  char name[str_size];
};*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "useful.h"

/* These are used to reset identifiers when no nodes or lists exist.
   They are not thread safe. It's just for convenience and pretty.
   See the note in node_kill for details */
int existing_nodes = 0;
int existing_lists = 0;

Node node_new(Node next, Node prev, void *v, int (*free)(void *), int (*_print)(void *))
{
  static int i = 1;
  Node new;

  if(NULL == (new = calloc(1, sizeof(struct _node))))
  {
    return NULL;
  }

  if(existing_nodes == 0) i = 1;

  new->next = next;
  new->prev = prev;
  new->v = v;
  new->free = free;
  new->print = _print;
  new->id = i++;
  new->n = 0;
  memset(new->str, 0, str_size);

  existing_nodes++;

  return new;
}


int node_kill(Node n)
{
  int result = failure;

  if(NULL == n)
  {
    return got_null;
  }

  existing_nodes--;
  /* This global variable resets the set of identifiers for nodes.

    Every successful creation increases it, and every deletion
    decreases it. It's mostly to make results more intuitive when
    running a program that regularly makes and destroys lists -
    at the point that no nodes exist, id's start from one again. */
  /* This is not thread safe. */

  if(NULL != n->free && NULL != n->v)
  {
    result = n->free(n->v);
  }
  else
  {
    result = success;
  }

  n->next = NULL;
  n->prev = NULL;
  n->free = NULL;
  n->print = NULL;
  n->v = NULL;
  n->n = 0;
  memset(n->str, 0, str_size);

  free(n);

  return result;
}


List list_new()
{
  static int i = 1;
  List l;

  if(NULL == (l = calloc(1, sizeof(struct _list))))
  {
    return NULL;
  }

  /* See the note for existing nodes in node_kill */
  if(existing_lists == 0) i = 1;

  l->root = NULL;
  l->current = NULL;
  l->size = 0;
  l->id = i++;
  memset(l->name, 0, str_size);

  existing_lists++;

  return l;
}


int list_kill(List l)
{
  if(NULL == l)
  {
    return got_null;
  }

  /* See the note for existing nodes in node_kill */
  existing_lists--;

  l->current = l->root;
  while(got_null != list_remove(l));

  l->root = NULL;
  l->current = NULL;
  l->size = 0;
  l->id = 0;
  memset(l->name, 0, str_size);

  free(l);

  return success;
}


int list_add_front(List l, Node n)
{
  if(NULL == l || NULL == n)
  {
    return got_null;
  }

  n->next = l->root;
  /* If there are no elements in the list, root is null. */
  if(NULL != l->root)
    l->root->prev = n;
  l->root = n;

  /* make sure the list is in a good state for use */
  l->size += 1;
  if(1 == l->size)
    l->current = l->root;

  return success;
}


int list_add_back(List l, Node n)
{
  Node temp = NULL;

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
    /* save current node's position */
    temp = l->current;  
    l->current = l->root;

    /* find the end of the list */
    while(l->current->next != NULL)
    {
      l->current = l->current->next;
    }

    /* link n into the end */
    l->current->next = n;
    n->prev = l->current;

    /* restore current node */
    l->current = temp;
  }

  l->size += 1;

  return success;
}


int list_add_before(List l, Node n)
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
    list_add_front(l, n);
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


int list_add_after(List l, Node n)
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
    list_add_back(l, n);
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


int list_remove(List l)
{
  Node temp;

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
    node_kill(l->current);
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
    node_kill(temp);
    
    /* if we didn't just remove the tail, fix the next node's prev */
    if(l->current->next != NULL)
      l->current->next->prev = l->current;
  }

  l->size -= 1;

  return success;
}


int list_root(List l)
{
  if(NULL == l || NULL ==  l->root)
  {
    return got_null;
  }

  l->current = l->root;

  return success;
}


int list_tail(List l)
{
  if(NULL == l || NULL ==  l->root)
  {
    return got_null;
  }

  while(success == list_next(l));

  return success;
}


int list_next(List l)
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


int list_prev(List l)
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


Node list_get(List l)
{
  if(NULL == l || NULL == l->current)
  {
    return NULL;
  }

  return l->current;
}

int list_print(List l)
{
  Node temp;
  printf("list has %d elements\n", l->size);

  if(NULL == l || NULL == l->current)
  {
    return got_null;
  }

  temp = l->current;
  l->current = l->root;
  while(l->current != NULL)
  //while(success == list_next(l))
  {
    printf("-%d-\n", l->current->id);
    l->current = l->current->next;
    /*printer = get_node(l);
    printer->print(printer);*/
  }
  l->current = temp;

  return success;
}
