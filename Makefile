.PHONY: test clean
all:

clean:
	rm -f ./test_avl_tree
	rm -f ./test_hash_table
	rm -f ./test_heap

test: test_avl_tree test_hash_table test_heap
	valgrind ./test_avl_tree
	valgrind ./test_hash_table
	valgrind ./test_heap

test_avl_tree: test_avl_tree.o
	$(CC) -o $@ test_avl_tree.o $(CFLAGS) $(LDFLAGS)

test_hash_table: test_hash_table.o
	$(CC) -o $@ test_hash_table.o $(CFLAGS) $(LDFLAGS)

test_heap: test_heap.o
	$(CC) -o $@ test_heap.o $(CFLAGS) $(LDFLAGS)
