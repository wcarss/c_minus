#ifndef _wlist
#define _wlist

typedef struct _list list;
typedef struct _list * List;

typedef struct _node node;
typedef struct _node * Node;

#define str_size 256
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
  struct _node *root;
  struct _node *current;
  int size;
  int id;
  char name[str_size];
};

Node node_new(Node next, Node prev, void *v, int (*free)(void *), int (*print)(void *));
int node_kill(Node n);

List list_new();
int list_kill(List l);

int list_add_front(List l, Node n);
int list_add_back(List l, Node n);
int list_add_before(List l, Node n);
int list_add_after(List l, Node n);
int list_remove(List l);
int list_root(List l);
int list_tail(List l);
int list_next(List l);
int list_prev(List l);
Node list_get(List l);
int list_print(List l);

#endif
