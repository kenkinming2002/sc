#include <assert.h>
#include <stdlib.h>

#include "utils.h"
#include "hash_table.h"

struct item
{
  struct sc_hash_table_node node;

  int key;
  int value;
};

static sc_hash_table_key_t item_key(struct sc_hash_table_node *node)
{
  struct item *item = SC_CONTAINER_OF(node, struct item, node);
  return (sc_hash_table_key_t)&item->key;
}

static size_t item_hash(sc_hash_table_key_t _key1)
{
  int *key1 = (int *)_key1;
  return *key1;
}

static int item_compare(sc_hash_table_key_t _key1, sc_hash_table_key_t _key2)
{
  int *key1 = (int *)_key1;
  int *key2 = (int *)_key2;
  return *key1 - *key2;
}

static void item_dispose(struct sc_hash_table_node *node)
{
  struct item *item = SC_CONTAINER_OF(node, struct item, node);
  free(item);
}

const struct sc_hash_table_ops ITEM_HASH_TABLE_OPS = {
  .key     = &item_key,
  .hash    = &item_hash,
  .compare = &item_compare,
  .dispose = &item_dispose,
};

int main()
{
  struct sc_hash_table hash_table = SC_HASH_TABLE_INIT(&ITEM_HASH_TABLE_OPS);

  // 1: Insertion
  for(int i=0; i<100; ++i)
  {
    struct item *item;
    item = malloc(sizeof *item);
    item->key   = 2 * i;
    item->value = 2 * i;
    sc_hash_table_insert(&hash_table, &item->node);
  }

  // 2: Looking up existent node
  for(int i=0; i<100; ++i)
    assert(sc_hash_table_lookup(&hash_table, SC_HASH_TABLE_KEY(int, 2 *i)));

  // 3: Looking up non-existent node
  for(int i=0; i<100; ++i)
    assert(!sc_hash_table_lookup(&hash_table, SC_HASH_TABLE_KEY(int, 2 * i + 1)));

  // 3: Removing existent node
  for(int i=21; i<47; ++i)
  {
    struct sc_hash_table_node *node = sc_hash_table_remove(&hash_table, SC_HASH_TABLE_KEY(int, 2 * i));
    assert(node);
    free(node);
  }

  // 4: Removing non-existent node
  for(int i=53; i<87; ++i)
  {
    struct sc_hash_table_node *node = sc_hash_table_remove(&hash_table, SC_HASH_TABLE_KEY(int, 2 * i + 1));
    assert(!node);
  }

  // 5: Dispose
  sc_hash_table_dispose(&hash_table);
}

