#ifndef SC_HEAP_H
#define SC_HEAP_H

#include <stddef.h>

// The compare function compare should return non-zero value if the item
// pointed to by the first argument should be placed above the item pointed to
// by the second argument, or zero otherwise.
//
// The update function is called whenever the index of an item has been changed.
// This can be useful in implementing an efficient decrease key operation.
struct sc_heap_ops
{
  int(*compare)(void *, void *);
  void(*update)(void *, size_t);
};

struct sc_heap
{
  struct sc_heap_ops  *ops;
  void               **items;
  size_t               item_count;
  size_t               item_capacity;
};

#define SC_HEAP_INIT(_ops) { .ops = _ops, .items = NULL, .item_count = 0, .item_capacity = 0, }

void sc_heap_bubble_up(struct sc_heap *heap, size_t k);
void sc_heap_bubble_down(struct sc_heap *heap, size_t k);

void sc_heap_build_push(struct sc_heap *heap, void *item);
void sc_heap_build_end(struct sc_heap *heap);

void sc_heap_push(struct sc_heap *heap, void *item);
void *sc_heap_pop(struct sc_heap *heap);

void sc_heap_dispose(struct sc_heap *heap);

#endif // SC_HEAP_H
