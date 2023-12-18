// Before including this header, the following should be defined:
//  - SC_HEAP_PREFIX
//  - SC_HEAP_TYPE
#include <stddef.h>

/*****************
 * Helper Macros *
 *****************/
#define SC_HEAP_CONCAT2_INNER(a, b) a ## b
#define SC_HEAP_CONCAT2(a, b) SC_HEAP_CONCAT2_INNER(a, b)

#define SC_HEAP_CONCAT3_INNER(a, b, c) a ## b ## c
#define SC_HEAP_CONCAT3(a, b, c) SC_HEAP_CONCAT3_INNER(a, b, c)

#define SC_HEAP_MAKE_NAME(name)          SC_HEAP_CONCAT3(SC_HEAP_PREFIX, _, name)
#define SC_HEAP_MAKE_NAME_INTERNAL(name) SC_HEAP_CONCAT2(_, SC_HEAP_MAKE_NAME(name))

/*********
 * Hooks *
 *********/
int SC_HEAP_MAKE_NAME(compare)(SC_HEAP_TYPE *, SC_HEAP_TYPE *);
void SC_HEAP_MAKE_NAME(update)(SC_HEAP_TYPE *, size_t);

/*********
 * Types *
 *********/
struct SC_HEAP_MAKE_NAME(heap)
{
  void   **items;
  size_t   item_count;
  size_t   item_capacity;
};

/*************
 * Functions *
 *************/
void SC_HEAP_MAKE_NAME(heap_init)(struct SC_HEAP_MAKE_NAME(heap) *heap);
void SC_HEAP_MAKE_NAME(heap_bubble_up)(struct SC_HEAP_MAKE_NAME(heap) *heap, size_t k);
void SC_HEAP_MAKE_NAME(heap_bubble_down)(struct SC_HEAP_MAKE_NAME(heap) *heap, size_t k);
void SC_HEAP_MAKE_NAME(heap_build_push)(struct SC_HEAP_MAKE_NAME(heap) *heap, void *item);
void SC_HEAP_MAKE_NAME(heap_build_end)(struct SC_HEAP_MAKE_NAME(heap) *heap);
void SC_HEAP_MAKE_NAME(heap_push)(struct SC_HEAP_MAKE_NAME(heap) *heap, void *item);
void *SC_HEAP_MAKE_NAME(heap_pop)(struct SC_HEAP_MAKE_NAME(heap) *heap);
void SC_HEAP_MAKE_NAME(heap_dispose)(struct SC_HEAP_MAKE_NAME(heap) *heap);

/*******************
 * Implementations *
 *******************/
#ifdef SC_HEAP_IMPLEMENTATION
#include <assert.h>
#include <stdlib.h>

void SC_HEAP_MAKE_NAME(heap_init)(struct SC_HEAP_MAKE_NAME(heap) *heap)
{
  heap->items         = NULL;
  heap->item_count    = 0;
  heap->item_capacity = 0;
}

void SC_HEAP_MAKE_NAME(heap_bubble_up)(struct SC_HEAP_MAKE_NAME(heap) *heap, size_t k)
{
  if(k == 0)
    return;

  size_t parent = (k - 1) >> 1;
  if(!SC_HEAP_MAKE_NAME(compare)(heap->items[parent], heap->items[k]))
  {
    void *tmp           = heap->items[parent];
    heap->items[parent] = heap->items[k];
    heap->items[k]      = tmp;

    SC_HEAP_MAKE_NAME(update)(heap->items[parent], parent);
    SC_HEAP_MAKE_NAME(update)(heap->items[k],      k);
    SC_HEAP_MAKE_NAME(heap_bubble_up)(heap, parent);
  }
}

void SC_HEAP_MAKE_NAME(heap_bubble_down)(struct SC_HEAP_MAKE_NAME(heap) *heap, size_t k)
{
  size_t top = k;
  size_t child_left  = (k << 1) + 1;
  size_t child_right = (k << 1) + 2;

  if(child_left  < heap->item_count && !SC_HEAP_MAKE_NAME(compare)(heap->items[top], heap->items[child_left]))  top = child_left;
  if(child_right < heap->item_count && !SC_HEAP_MAKE_NAME(compare)(heap->items[top], heap->items[child_right])) top = child_right;

  if(top != k)
  {
    void *tmp        = heap->items[top];
    heap->items[top] = heap->items[k];
    heap->items[k]   = tmp;

    SC_HEAP_MAKE_NAME(update)(heap->items[top], top);
    SC_HEAP_MAKE_NAME(update)(heap->items[k],   k);
    SC_HEAP_MAKE_NAME(heap_bubble_down)(heap, top);
  }
}

void SC_HEAP_MAKE_NAME(heap_build_push)(struct SC_HEAP_MAKE_NAME(heap) *heap, void *item)
{
  if(heap->item_capacity == heap->item_count)
  {
    heap->item_capacity = heap->item_capacity != 0 ? heap->item_capacity * 2 : 1;
    heap->items         = reallocarray(heap->items, heap->item_capacity, sizeof heap->items[0]);
  }
  heap->items[heap->item_count++] = item;
  SC_HEAP_MAKE_NAME(update)(heap->items[heap->item_count-1], heap->item_count-1);
}

void SC_HEAP_MAKE_NAME(heap_build_end)(struct SC_HEAP_MAKE_NAME(heap) *heap)
{
  for(size_t i=heap->item_count-1; i<heap->item_count; --i)
    SC_HEAP_MAKE_NAME(heap_bubble_down)(heap, i);
}

void SC_HEAP_MAKE_NAME(heap_push)(struct SC_HEAP_MAKE_NAME(heap) *heap, void *item)
{
  if(heap->item_capacity == heap->item_count)
  {
    heap->item_capacity = heap->item_capacity != 0 ? heap->item_capacity * 2 : 1;
    heap->items         = reallocarray(heap->items, heap->item_capacity, sizeof heap->items[0]);
  }
  heap->items[heap->item_count++] = item;
  SC_HEAP_MAKE_NAME(update)(heap->items[heap->item_count-1], heap->item_count-1);
  SC_HEAP_MAKE_NAME(heap_bubble_up)(heap, heap->item_count - 1);
}

void *SC_HEAP_MAKE_NAME(heap_pop)(struct SC_HEAP_MAKE_NAME(heap) *heap)
{
  assert(heap->item_count != 0);
  void *item = heap->items[0];
  heap->items[0] = heap->items[--heap->item_count];
  SC_HEAP_MAKE_NAME(update)(heap->items[0], 0);
  SC_HEAP_MAKE_NAME(heap_bubble_down)(heap, 0);
  return item;
}

void SC_HEAP_MAKE_NAME(heap_dispose)(struct SC_HEAP_MAKE_NAME(heap) *heap)
{
  free(heap->items);
}

#endif
