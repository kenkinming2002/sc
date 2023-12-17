#include "heap.h"

#include <assert.h>
#include <string.h>

// My super duper naive implementation
//
// TODO: Optimize this?
static void memswap(void *p1, void *p2, size_t n)
{
  char *_p1 = p1;
  char *_p2 = p2;
  for(size_t i=0; i<n; ++i)
  {
    char b = _p1[i];
    _p1[i] = _p2[i];
    _p2[i] = b;
  }
}

void sc_heapify(void *base, size_t n, size_t size, int(*compare)(void *, void *))
{
  for(size_t k=n-1; k<n; --k)
    sc_heap_bubble_down(base, n, size, compare, k);
}

void sc_heap_push(void *base, size_t n, size_t size, int(*compare)(void *, void *), void *item)
{
  memcpy(base + size * n++, item, size);
  sc_heap_bubble_up(base, n, size, compare, n-1);
}

void sc_heap_pop(void *base, size_t n, size_t size, int(*compare)(void *, void *))
{
  if(n != 1)
  {
    memcpy(base, base + size * --n, size);
    sc_heap_bubble_down(base, n, size, compare, 0);
  }
}

void sc_heap_bubble_up(void *base, size_t n, size_t size, int(*compare)(void *, void *), size_t k)
{
  size_t  item_index = k;
  void   *item       = base + size * item_index;

  if(item_index == 0)
    return;

  size_t  parent_index = (k - 1) >> 1;
  void   *parent       = base + size * parent_index;
  if(!compare(parent, item))
  {
    memswap(parent, item, size);
    sc_heap_bubble_up(base, n, size, compare, parent_index);
  }
}

void sc_heap_bubble_down(void *base, size_t n, size_t size, int(*compare)(void *, void *), size_t k)
{
  size_t  parent_index = k;
  void   *parent       = base + size * parent_index;

  size_t  item_index = parent_index;
  void   *item       = parent;

  // Left Child
  {
    size_t  child_index = (k << 1) + 1;
    void   *child       = base + size * child_index;
    if(child_index < n && !compare(item, child))
    {
      item_index = child_index;
      item       = child;
    }
  }

  // Right Child
  {
    size_t  child_index = (k << 1) + 2;
    void   *child       = base + size * child_index;
    if(child_index < n && !compare(item, child))
    {
      item_index = child_index;
      item       = child;
    }
  }

  if(parent != item)
  {
    memswap(parent, item, size);
    sc_heap_bubble_down(base, n, size, compare, item_index);
  }
}
