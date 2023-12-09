#ifndef SC_HASH_TABLE_H
#define SC_HASH_TABLE_H

#include <stddef.h>

struct hash_table_node
{
  struct hash_table_node *next;
  size_t                  hash;
};

struct hash_table_bucket
{
  struct hash_table_node *head;
};

struct hash_table
{
  const struct hash_table_ops *ops;
  struct hash_table_bucket    *buckets;
  size_t                       bucket_count;
  size_t                       load;
};

typedef struct hash_table_key *hash_table_key_t;

struct hash_table_ops
{
  hash_table_key_t (*key)(struct hash_table_node *node);
  size_t(*hash)(hash_table_key_t key);
  int(*compare)(hash_table_key_t key1, hash_table_key_t key2);
  void (*dispose)(struct hash_table_node *node);
};

#define HASH_TABLE_INIT(_ops) { .ops = _ops, .buckets = NULL, .bucket_count = 0, .load = 0, }
#define HASH_TABLE_KEY(type, value) (hash_table_key_t)(&(struct { type _value; }){ ._value = (value) })

void hash_table_rehash(struct hash_table *hash_table, size_t bucket_count);

int hash_table_insert(struct hash_table *hash_table, struct hash_table_node *node);

struct hash_table_node *hash_table_remove(struct hash_table *hash_table, hash_table_key_t key);
struct hash_table_node *hash_table_lookup(struct hash_table *hash_table, hash_table_key_t key);

void hash_table_dispose(struct hash_table *hash_table);

#endif // SC_HASH_TABLE_H
