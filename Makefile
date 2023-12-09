.PHONY: test
all: libsc.a
libsc.a: hash_table.o
	$(AR) rcs $@ $?

clean:
	rm -f ./test_hash_table
	rm -f ./test_hash_table.o
	rm -f ./hash_table.o
	rm -f ./libsc.a

test: test_hash_table
	valgrind ./test_hash_table

test_hash_table: test_hash_table.o libsc.a
	$(CC) -o $@ test_hash_table.o -L. -lsc $(CFLAGS) $(LDFLAGS)
