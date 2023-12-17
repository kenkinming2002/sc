#include "heap.h"

#include <assert.h>
#include <stdio.h>

struct item
{
  unsigned value;
};

int item_compare(void *_item1, void *_item2)
{
  struct item *item1 = _item1;
  struct item *item2 = _item2;
  return item1->value < item2->value;
}

int main()
{
  struct item items[63];
  size_t      item_count = 0;

  // 1: Initializing items manually and calling heapify
  items[item_count++].value = 34;
  items[item_count++].value = 15;
  items[item_count++].value = 7;
  items[item_count++].value = 24;
  items[item_count++].value = 58;
  items[item_count++].value = 40;
  items[item_count++].value = 19;
  items[item_count++].value = 10;
  items[item_count++].value = 26;
  items[item_count++].value = 49;
  items[item_count++].value = 39;
  items[item_count++].value = 54;
  items[item_count++].value = 51;
  items[item_count++].value = 11;
  items[item_count++].value = 56;
  items[item_count++].value = 0;
  items[item_count++].value = 44;
  items[item_count++].value = 21;
  items[item_count++].value = 41;
  items[item_count++].value = 14;
  items[item_count++].value = 30;
  items[item_count++].value = 25;
  items[item_count++].value = 36;
  items[item_count++].value = 62;
  items[item_count++].value = 29;
  items[item_count++].value = 23;
  items[item_count++].value = 27;
  items[item_count++].value = 46;
  items[item_count++].value = 52;
  items[item_count++].value = 57;
  items[item_count++].value = 33;
  items[item_count++].value = 12;
  items[item_count++].value = 18;
  items[item_count++].value = 32;
  items[item_count++].value = 17;
  items[item_count++].value = 35;
  items[item_count++].value = 37;
  items[item_count++].value = 20;
  items[item_count++].value = 59;
  items[item_count++].value = 48;
  items[item_count++].value = 8;
  items[item_count++].value = 31;
  items[item_count++].value = 60;
  items[item_count++].value = 9;
  items[item_count++].value = 4;
  items[item_count++].value = 22;
  items[item_count++].value = 1;
  items[item_count++].value = 2;
  items[item_count++].value = 50;
  items[item_count++].value = 42;
  items[item_count++].value = 16;
  items[item_count++].value = 28;
  items[item_count++].value = 45;
  items[item_count++].value = 6;
  items[item_count++].value = 5;
  items[item_count++].value = 38;
  items[item_count++].value = 43;
  items[item_count++].value = 13;
  items[item_count++].value = 61;
  items[item_count++].value = 53;
  items[item_count++].value = 3;
  items[item_count++].value = 47;
  items[item_count++].value = 55;
  sc_heapify(items, item_count, sizeof items[0], &item_compare);

  for(unsigned i=0; i<63; ++i)
  {
    assert(items[0].value == i);
    sc_heap_pop(items, item_count--, sizeof items[0], &item_compare);
  }

  // 2: Pushing items onto the heap one by one
  for(unsigned i=0; i<63; ++i)
    sc_heap_push(items, item_count++, sizeof items[0], &item_compare, &(struct item){ .value = i });

  for(unsigned i=0; i<63; ++i)
  {
    assert(items[0].value == i);
    sc_heap_pop(items, item_count--, sizeof items[0], &item_compare);
  }
}

