#include <assert.h>
#include <stdlib.h>

#include "utils.h"
#include "hash_table.h"

struct item
{
  struct hash_table_node node;

  int key;
  int value;
};

static hash_table_key_t item_key(struct hash_table_node *node)
{
  struct item *item = container_of(node, struct item, node);
  return (hash_table_key_t)&item->key;
}

static size_t item_hash(hash_table_key_t _key1)
{
  int *key1 = (int *)_key1;
  return *key1;
}

static int item_compare(hash_table_key_t _key1, hash_table_key_t _key2)
{
  int *key1 = (int *)_key1;
  int *key2 = (int *)_key2;
  return *key1 == *key2;
}

static void item_dispose(struct hash_table_node *node)
{
  struct item *item = container_of(node, struct item, node);
  free(item);
}

const struct hash_table_ops ITEM_HASH_TABLE_OPS = {
  .key     = &item_key,
  .hash    = &item_hash,
  .compare = &item_compare,
  .dispose = &item_dispose,
};

int main()
{
  struct hash_table hash_table = HASH_TABLE_INIT(&ITEM_HASH_TABLE_OPS);

  // 1: Insertion
  for(int i=0; i<100; ++i)
  {
    struct item *item;
    item = malloc(sizeof *item);
    item->key   = 2 * i;
    item->value = 2 * i;
    hash_table_insert(&hash_table, &item->node);
  }

  // 2: Looking up existent node
  for(int i=0; i<100; ++i)
  {
    int key = 2 * i;
    assert(hash_table_lookup(&hash_table, (hash_table_key_t)&key));
  }

  // 3: Looking up non-existent node
  for(int i=0; i<100; ++i)
  {
    int key = 2 * i + 1;
    assert(!hash_table_lookup(&hash_table, (hash_table_key_t)&key));
  }

  // 3: Removing existent node
  for(int i=21; i<47; ++i)
  {
    int key = 2 * i;

    struct hash_table_node *node = hash_table_remove(&hash_table, (hash_table_key_t)&key);
    assert(node);
    free(node);
  }

  // 4: Removing non-existent node
  for(int i=53; i<87; ++i)
  {
    int key = 2 * i + 1;

    struct hash_table_node *node = hash_table_remove(&hash_table, (hash_table_key_t)&key);
    assert(!node);
  }

  // 5: Dispose
  hash_table_dispose(&hash_table);
}

