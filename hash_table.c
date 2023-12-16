#include "hash_table.h"

#include <stdlib.h>

void sc_hash_table_rehash(struct sc_hash_table *hash_table, size_t bucket_count)
{
  if(hash_table->bucket_count >= bucket_count)
    return;

  hash_table->buckets = reallocarray(hash_table->buckets, bucket_count, sizeof hash_table->buckets[0]);
  for(size_t i=hash_table->bucket_count; i<bucket_count; ++i)
    hash_table->buckets[i].head = NULL;
  hash_table->bucket_count = bucket_count;

  struct sc_hash_table_node *orphans = NULL;
  for(size_t i=0; i<hash_table->bucket_count; ++i)
  {
    struct sc_hash_table_bucket *bucket = &hash_table->buckets[i];
    struct sc_hash_table_node   *prev   = (struct sc_hash_table_node *)bucket;
    struct sc_hash_table_node   *node   = prev->next;
    while(node)
      if(node->hash % hash_table->bucket_count == i)
      {
        prev = node;
        node = node->next;
      }
      else
      {
        struct sc_hash_table_node *orphan = node;
        prev->next   = node->next;
        node         = node->next;
        orphan->next = orphans;
        orphans      = orphan;
      }
  }

  while(orphans)
  {
    struct sc_hash_table_node *orphan = orphans;
    orphans = orphans->next;

    struct sc_hash_table_bucket *bucket = &hash_table->buckets[orphan->hash % hash_table->bucket_count];
    orphan->next = bucket->head;
    bucket->head = orphan;
  }
}

int sc_hash_table_insert(struct sc_hash_table *hash_table, struct sc_hash_table_node *new_node)
{
  if(hash_table->bucket_count == 0)
    sc_hash_table_rehash(hash_table, 8);
  else if(2 * hash_table->load > 3 * hash_table->bucket_count)
    sc_hash_table_rehash(hash_table, 2 * hash_table->bucket_count);

  size_t hash = hash_table->ops->hash(hash_table->ops->key(new_node));

  struct sc_hash_table_bucket *bucket = &hash_table->buckets[hash % hash_table->bucket_count];
  for(struct sc_hash_table_node *node = bucket->head; node; node = node->next)
    if(node->hash == hash && hash_table->ops->compare(hash_table->ops->key(node), hash_table->ops->key(new_node)) == 0)
      return 0;

  ++hash_table->load;
  new_node->hash = hash;
  new_node->next = bucket->head;
  bucket->head = new_node;
  return 1;
}

struct sc_hash_table_node *sc_hash_table_lookup(struct sc_hash_table *hash_table, sc_hash_table_key_t key)
{
  size_t hash = hash_table->ops->hash(key);

  struct sc_hash_table_bucket *bucket = &hash_table->buckets[hash % hash_table->bucket_count];
  for(struct sc_hash_table_node *node = bucket->head; node; node = node->next)
    if(node->hash == hash && hash_table->ops->compare(hash_table->ops->key(node), key) == 0)
      return node;

  return NULL;
}

struct sc_hash_table_node *sc_hash_table_remove(struct sc_hash_table *hash_table, sc_hash_table_key_t key)
{
  size_t hash = hash_table->ops->hash(key);

  struct sc_hash_table_bucket *bucket = &hash_table->buckets[hash % hash_table->bucket_count];
  struct sc_hash_table_node   *prev   = (struct sc_hash_table_node *)bucket;
  struct sc_hash_table_node   *node   = prev->next;
  for(; node; prev = node, node = node->next)
    if(node->hash == hash && hash_table->ops->compare(hash_table->ops->key(node), key) == 0)
    {
      struct sc_hash_table_node *orphan = node;
      prev->next   = node->next;
      node         = node->next;
      orphan->next = NULL;

      --hash_table->load;
      return orphan;
    }

  return NULL;
}

void sc_hash_table_dispose(struct sc_hash_table *hash_table)
{
  for(size_t i=0; i<hash_table->bucket_count; ++i)
  {
    struct sc_hash_table_bucket *bucket = &hash_table->buckets[i];
    struct sc_hash_table_node *node = bucket->head;
    while(node)
    {
      struct sc_hash_table_node *next = node->next;
      hash_table->ops->dispose(node);
      node = next;
    }
  }
  free(hash_table->buckets);

  hash_table->buckets      = 0;
  hash_table->bucket_count = 0;
  hash_table->load         = 0;
}

