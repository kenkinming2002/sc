#include <assert.h>
#include <stdlib.h>

#include "utils.h"
#include "avl_tree.h"

struct item
{
  struct avl_tree_node node;

  int key;
  int value;
};

static avl_tree_key_t item_key(struct avl_tree_node *node)
{
  struct item *item = container_of(node, struct item, node);
  return (avl_tree_key_t)&item->key;
}

static int item_compare(avl_tree_key_t _key1, avl_tree_key_t _key2)
{
  int *key1 = (int *)_key1;
  int *key2 = (int *)_key2;
  return *key1 - *key2;
}

static void item_dispose(struct avl_tree_node *node)
{
  struct item *item = container_of(node, struct item, node);
  free(item);
}

const struct avl_tree_ops ITEM_AVL_TREE_OPS = {
  .key     = &item_key,
  .compare = &item_compare,
  .dispose = &item_dispose,
};

int main()
{
  struct avl_tree avl_tree = AVL_TREE_INIT(&ITEM_AVL_TREE_OPS);

  // 1: Insertion
  for(int i=0; i<100; ++i)
  {
    struct item *item;
    item = malloc(sizeof *item);
    item->key   = 2 * i;
    item->value = 2 * i;
    avl_tree_insert(&avl_tree, &item->node);
    avl_tree_check(&avl_tree);
  }

  // 2: Looking up existent node
  for(int i=0; i<100; ++i)
    assert(avl_tree_lookup(&avl_tree, AVL_TREE_KEY(int, 2 *i)));

  // 3: Looking up non-existent node
  for(int i=0; i<100; ++i)
    assert(!avl_tree_lookup(&avl_tree, AVL_TREE_KEY(int, 2 * i + 1)));

  // 3: Removing existent node
  for(int i=21; i<47; ++i)
  {
    struct avl_tree_node *node = avl_tree_remove(&avl_tree, AVL_TREE_KEY(int, 2 * i));
    assert(node);
    free(node);

    avl_tree_check(&avl_tree);
  }

  // 4: Removing non-existent node
  for(int i=53; i<87; ++i)
  {
    struct avl_tree_node *node = avl_tree_remove(&avl_tree, AVL_TREE_KEY(int, 2 * i + 1));
    assert(!node);

    avl_tree_check(&avl_tree);
  }

  // 5: Dispose
  avl_tree_dispose(&avl_tree);
}
