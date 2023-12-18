#include <stddef.h>

struct item
{
  size_t   index;
  unsigned value;
};

int item_compare(struct item *item1, struct item *item2) { return item1->value < item2->value; }
void item_update(struct item *item, size_t index) { item->index = index; }

#define SC_HEAP_PREFIX item
#define SC_HEAP_TYPE   struct item
#define SC_HEAP_IMPLEMENTATION
#include "heap.h"

#include <assert.h>
#include <stdlib.h>

void check_item_heap(struct item_heap *heap)
{
  for(size_t i=0; i<heap->item_count; ++i)
  {
    struct item *item = heap->items[i];
    assert(item->index == i);
  }
}

int main()
{
  struct item_heap heap;

  // 1: Push/Pop
  item_heap_init(&heap);
  for(unsigned i=63; i<64; --i)
  {
    struct item *item = malloc(sizeof *item);
    item->value = i;
    item_heap_push(&heap, item);
    check_item_heap(&heap);
  }
  for(unsigned i=0; i<64; ++i)
  {
    struct item *item = item_heap_pop(&heap);
    assert(item->value == i);
    free(item);
    check_item_heap(&heap);
  }
  item_heap_dispose(&heap);

  // 2: Build
  item_heap_init(&heap);
  for(unsigned i=63; i<64; --i)
  {
    struct item *item = malloc(sizeof *item);
    item->value = i;
    item_heap_build_push(&heap, item);
    check_item_heap(&heap);
  }
  item_heap_build_end(&heap);
  check_item_heap(&heap);
  for(unsigned i=0; i<64; ++i)
  {
    struct item *item = item_heap_pop(&heap);
    assert(item->value == i);
    free(item);
    check_item_heap(&heap);
  }
  item_heap_dispose(&heap);
}

