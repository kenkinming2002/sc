#ifndef SC_AVL_TREE_H
#define SC_AVL_TREE_H

#include <stddef.h>

struct avl_tree_node
{
  struct avl_tree_node *left;
  struct avl_tree_node *right;
  int                   bf; // TODO: Consider tagged pointer
};

struct avl_tree
{
  const struct avl_tree_ops *ops;
  struct avl_tree_node      *root;
};

typedef struct avl_tree_key *avl_tree_key_t;

struct avl_tree_ops
{
  avl_tree_key_t (*key)(struct avl_tree_node *node);
  int(*compare)(avl_tree_key_t key1, avl_tree_key_t key2);
  void (*dispose)(struct avl_tree_node *node);
};

#define AVL_TREE_INIT(_ops) { .ops = _ops, .root = NULL }
#define AVL_TREE_KEY(type, value) (avl_tree_key_t)(&(struct { type _value; }){ ._value = (value) })

void avl_tree_check(struct avl_tree *avl_tree);

int avl_tree_insert(struct avl_tree *avl_tree, struct avl_tree_node *node);

struct avl_tree_node *avl_tree_lookup(struct avl_tree *avl_tree, avl_tree_key_t key);
struct avl_tree_node *avl_tree_remove(struct avl_tree *avl_tree, avl_tree_key_t key);

void avl_tree_dispose(struct avl_tree *avl_tree);

#endif // SC_AVL_TREE_H
