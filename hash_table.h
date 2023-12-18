#ifndef SC_HASH_TABLE_H
#define SC_HASH_TABLE_H

#include <stddef.h>

struct sc_hash_table_node
{
  struct sc_hash_table_node *next;
  size_t                  hash;
};

struct sc_hash_table_bucket
{
  struct sc_hash_table_node *head;
};

struct sc_hash_table
{
  const struct sc_hash_table_ops *ops;
  struct sc_hash_table_bucket    *buckets;
  size_t                          bucket_count;
  size_t                          load;
};

typedef struct sc_hash_table_key *sc_hash_table_key_t;

struct sc_hash_table_ops
{
  sc_hash_table_key_t (*key)(struct sc_hash_table_node *node);
  size_t(*hash)(sc_hash_table_key_t key);
  int(*compare)(sc_hash_table_key_t key1, sc_hash_table_key_t key2);
  void (*dispose)(struct sc_hash_table_node *node);
};

#define SC_HASH_TABLE_INIT(_ops) { .ops = _ops, .buckets = NULL, .bucket_count = 0, .load = 0, }
#define SC_HASH_TABLE_KEY(type, value) (sc_hash_table_key_t)(&(struct { type _value; }){ ._value = (value) })

void sc_hash_table_rehash(struct sc_hash_table *hash_table, size_t bucket_count);

int sc_hash_table_insert(struct sc_hash_table *hash_table, struct sc_hash_table_node *node);

struct sc_hash_table_node *sc_hash_table_lookup(struct sc_hash_table *hash_table, sc_hash_table_key_t key);
struct sc_hash_table_node *sc_hash_table_remove(struct sc_hash_table *hash_table, sc_hash_table_key_t key);

void sc_hash_table_dispose(struct sc_hash_table *hash_table);

#endif // SC_HASH_TABLE_H
