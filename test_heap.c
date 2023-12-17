#include "heap.h"

#include <assert.h>
#include <stdlib.h>

struct item
{
  size_t   index;
  unsigned value;
};

int item_compare(void *_item1, void *_item2)
{
  struct item *item1 = _item1;
  struct item *item2 = _item2;
  return item1->value < item2->value;
}

void item_update(void *_item, size_t index)
{
  struct item *item = _item;
  item->index = index;
}

struct sc_heap_ops ITEM_OPS = {
  .compare = &item_compare,
  .update  = &item_update,
};

void check_item_heap(struct sc_heap *heap)
{
  for(size_t i=0; i<heap->item_count; ++i)
  {
    struct item *item = heap->items[i];
    assert(item->index == i);
  }
}

int main()
{
  // 1: Push/Pop
  {
    struct sc_heap heap = SC_HEAP_INIT(&ITEM_OPS);
    for(unsigned i=63; i<64; --i)
    {
      struct item *item = malloc(sizeof *item);
      item->value = i;
      sc_heap_push(&heap, item);
      check_item_heap(&heap);
    }

    for(unsigned i=0; i<64; ++i)
    {
      struct item *item = sc_heap_pop(&heap);
      assert(item->value == i);
      free(item);
      check_item_heap(&heap);
    }
    sc_heap_dispose(&heap);
  }

  // 2: Build
  {
    struct sc_heap heap = SC_HEAP_INIT(&ITEM_OPS);
    for(unsigned i=63; i<64; --i)
    {
      struct item *item = malloc(sizeof *item);
      item->value = i;
      sc_heap_build_push(&heap, item);
      check_item_heap(&heap);
    }
    sc_heap_build_end(&heap);
    check_item_heap(&heap);

    for(unsigned i=0; i<64; ++i)
    {
      struct item *item = sc_heap_pop(&heap);
      assert(item->value == i);
      free(item);
      check_item_heap(&heap);
    }
    sc_heap_dispose(&heap);
  }
}

