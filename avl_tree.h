#ifndef SC_AVL_TREE_H
#define SC_AVL_TREE_H

#include <stddef.h>

struct sc_avl_tree_node
{
  struct sc_avl_tree_node *left;
  struct sc_avl_tree_node *right;
  int                      bf; // TODO: Consider tagged pointer
};

struct sc_avl_tree
{
  const struct sc_avl_tree_ops *ops;
  struct sc_avl_tree_node      *root;
};

typedef struct sc_avl_tree_key *sc_avl_tree_key_t;

struct sc_avl_tree_ops
{
  sc_avl_tree_key_t (*key)(struct sc_avl_tree_node *node);
  int(*compare)(sc_avl_tree_key_t key1, sc_avl_tree_key_t key2);
  void (*dispose)(struct sc_avl_tree_node *node);
};

#define SC_AVL_TREE_INIT(_ops) { .ops = _ops, .root = NULL }
#define SC_AVL_TREE_KEY(type, value) (sc_avl_tree_key_t)(&(struct { type _value; }){ ._value = (value) })

void sc_avl_tree_check(struct sc_avl_tree *avl_tree);

int sc_avl_tree_insert(struct sc_avl_tree *avl_tree, struct sc_avl_tree_node *node);

struct sc_avl_tree_node *sc_avl_tree_lookup(struct sc_avl_tree *avl_tree, sc_avl_tree_key_t key);
struct sc_avl_tree_node *sc_avl_tree_remove(struct sc_avl_tree *avl_tree, sc_avl_tree_key_t key);

void sc_avl_tree_dispose(struct sc_avl_tree *avl_tree);

#endif // SC_AVL_TREE_H
