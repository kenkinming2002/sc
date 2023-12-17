#include "heap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void sc_heap_bubble_up(struct sc_heap *heap, size_t k)
{
  if(k == 0)
    return;

  size_t parent = (k - 1) >> 1;
  if(!heap->ops->compare(heap->items[parent], heap->items[k]))
  {
    void *tmp           = heap->items[parent];
    heap->items[parent] = heap->items[k];
    heap->items[k]      = tmp;

    heap->ops->update(heap->items[parent], parent);
    heap->ops->update(heap->items[k],      k);
    sc_heap_bubble_up(heap, parent);
  }
}

void sc_heap_bubble_down(struct sc_heap *heap, size_t k)
{
  size_t top = k;
  size_t child_left  = (k << 1) + 1;
  size_t child_right = (k << 1) + 2;

  if(child_left  < heap->item_count && !heap->ops->compare(heap->items[top], heap->items[child_left]))  top = child_left;
  if(child_right < heap->item_count && !heap->ops->compare(heap->items[top], heap->items[child_right])) top = child_right;

  if(top != k)
  {
    void *tmp        = heap->items[top];
    heap->items[top] = heap->items[k];
    heap->items[k]   = tmp;

    heap->ops->update(heap->items[top], top);
    heap->ops->update(heap->items[k],   k);
    sc_heap_bubble_down(heap, top);
  }
}

void sc_heap_build_push(struct sc_heap *heap, void *item)
{
  if(heap->item_capacity == heap->item_count)
  {
    heap->item_capacity = heap->item_capacity != 0 ? heap->item_capacity * 2 : 1;
    heap->items         = reallocarray(heap->items, heap->item_capacity, sizeof heap->items[0]);
  }
  heap->items[heap->item_count++] = item;
  heap->ops->update(heap->items[heap->item_count-1], heap->item_count-1);
}

void sc_heap_build_end(struct sc_heap *heap)
{
  for(size_t i=heap->item_count-1; i<heap->item_count; --i)
    sc_heap_bubble_down(heap, i);
}

void sc_heap_push(struct sc_heap *heap, void *item)
{
  if(heap->item_capacity == heap->item_count)
  {
    heap->item_capacity = heap->item_capacity != 0 ? heap->item_capacity * 2 : 1;
    heap->items         = reallocarray(heap->items, heap->item_capacity, sizeof heap->items[0]);
  }
  heap->items[heap->item_count++] = item;
  heap->ops->update(heap->items[heap->item_count-1], heap->item_count-1);
  sc_heap_bubble_up(heap, heap->item_count - 1);
}

void *sc_heap_pop(struct sc_heap *heap)
{
  assert(heap->item_count != 0);
  void *item = heap->items[0];
  heap->items[0] = heap->items[--heap->item_count];
  heap->ops->update(heap->items[0], 0);
  sc_heap_bubble_down(heap, 0);
  return item;
}

void sc_heap_dispose(struct sc_heap *heap)
{
  free(heap->items);
}
