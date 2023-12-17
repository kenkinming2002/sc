#ifndef SC_HEAP_H
#define SC_HEAP_H

#include <stddef.h>

// Heap implementation
//
// The comparison function compare should return non-zero value if the item
// pointed to by the first argument should be placed above the item pointed to
// by the second argument, or zero otherwise.

void sc_heapify(void *base, size_t n, size_t size, int(*compare)(void *, void *));

void sc_heap_push(void *base, size_t n, size_t size, int(*compare)(void *, void *), void *item);
void sc_heap_pop(void *base, size_t n, size_t size, int(*compare)(void *, void *));

void sc_heap_bubble_up(void *base, size_t n, size_t size, int(*compare)(void *, void *), size_t k);
void sc_heap_bubble_down(void *base, size_t n, size_t size, int(*compare)(void *, void *), size_t k);

#endif // SC_HEAP_H
