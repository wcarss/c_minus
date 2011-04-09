#ifndef _whlist
#define _whlist

typedef struct _hlist hlist;
typedef struct _hlist * HList;

typedef struct _hnode hnode;
typedef struct _hnode * HNode;

#define str_size 256
struct _hnode
{
  struct _hnode *next;
  struct _hnode *prev;
  int id;
  int value;
  int lineno;
  int colno;
  double double_value;
  char key[str_size];
  char type[str_size];
};

struct _hlist
{
  struct _hnode *root;
  struct _hnode *current;
  int size;
  int id;
  char name[str_size];
};

HNode hnode_new(char *key, char *type, int lineno, int colno, int value);
int hnode_kill(HNode n);

HList hlist_new();
int hlist_kill(HList l);

int hlist_add_front(HList l, HNode n);
int hlist_add_back(HList l, HNode n);
int hlist_add_before(HList l, HNode n);
int hlist_add_after(HList l, HNode n);
int hlist_remove(HList l);
int hlist_root(HList l);
int hlist_tail(HList l);
int hlist_next(HList l);
int hlist_prev(HList l);
HNode hlist_get(HList l);
int hlist_print(HList l);

#endif
