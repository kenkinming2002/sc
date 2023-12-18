#include <assert.h>
#include <stdlib.h>

struct item
{
  struct item *next;
  size_t       hash;

  int key;
  int value;
};

static int item_key(struct item *item)
{
  return item->key;
}

static size_t item_hash(int key)
{
  return key;
}

static int item_compare(int key1, int key2)
{
  return key1 - key2;
}

static void item_dispose(struct item *item)
{
  free(item);
}

#define SC_HASH_TABLE_PREFIX    item
#define SC_HASH_TABLE_NODE_TYPE struct item
#define SC_HASH_TABLE_KEY_TYPE  int
#define SC_HASH_TABLE_IMPLEMENTATION
#include "hash_table.h"

int main()
{
  struct item_hash_table items;
  item_hash_table_init(&items);

  // 1: Insertion
  for(int i=0; i<100; ++i)
  {
    struct item *item;
    item = malloc(sizeof *item);
    item->key   = 2 * i;
    item->value = 2 * i;
    item_hash_table_insert(&items, item);
  }

  // 2: Looking up existent node
  for(int i=0; i<100; ++i)
    assert(item_hash_table_lookup(&items, 2 * i));

  // 3: Looking up non-existent node
  for(int i=0; i<100; ++i)
    assert(!item_hash_table_lookup(&items, 2 * i + 1));

  // 3: Removing existent node
  for(int i=21; i<47; ++i)
  {
    struct item *item = item_hash_table_remove(&items, 2 * i);
    assert(item);
    free(item);
  }

  // 4: Removing non-existent node
  for(int i=53; i<87; ++i)
  {
    struct item *item = item_hash_table_remove(&items, 2 * i + 1);
    assert(!item);
  }

  // 5: Dispose
  item_hash_table_dispose(&items);
}

