#include <assert.h>
#include <stdlib.h>

struct item
{
  struct item *left;
  struct item *right;
  int          bf;

  int key;
  int value;
};

static inline int item_key(struct item *item) { return item->key; }
static inline int item_key_compare(int key1, int key2) { return key1 - key2; }
static inline void item_dispose(struct item *item) { free(item); }

#define SC_AVL_TREE_PREFIX    item
#define SC_AVL_TREE_NODE_TYPE struct item
#define SC_AVL_TREE_KEY_TYPE  int
#define SC_AVL_TREE_IMPLEMENTATION
#include "avl_tree.h"

int main()
{
  struct item_avl_tree items;
  item_avl_tree_init(&items);

  // 1: Insertion
  for(int i=0; i<100; ++i)
  {
    struct item *item;
    item = malloc(sizeof *item);
    item->key   = 2 * i;
    item->value = 2 * i;
    item_avl_tree_insert(&items, item);
    item_avl_tree_check(&items);
  }

  // 2: Looking up existent node
  for(int i=0; i<100; ++i)
    assert(item_avl_tree_lookup(&items, 2 * i));

  // 3: Looking up non-existent node
  for(int i=0; i<100; ++i)
    assert(!item_avl_tree_lookup(&items, 2 * i + 1));

  // 3: Removing existent node
  for(int i=21; i<47; ++i)
  {
    struct item *item = item_avl_tree_remove(&items, 2 * i);
    assert(item);
    free(item);

    item_avl_tree_check(&items);
  }

  // 4: Removing non-existent node
  for(int i=53; i<87; ++i)
  {
    struct item *item = item_avl_tree_remove(&items, 2 * i + 1);
    assert(!item);
    item_avl_tree_check(&items);
  }

  // 5: Dispose
  item_avl_tree_dispose(&items);
}
