#include "test.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../mem.h"
#include "../mem_internals.h"

extern void* heap;

// Free 1 block from 2 blocks
bool test2() {
	printf(BLU "\n\n TEST n2 (freeing 1 block from 2 blocks) \n\n" RESET);

	size_t test_block_size = 1775; // American Revolution massive events
	void* test_block = _malloc(test_block_size, heap); 

	struct block_header *testblock = block_get_header(test_block);

	printf("Before freeing:\n");
  debug_heap(stdout, heap);
  _free(test_block);
	printf("After freeing:\n");
	debug_heap(stdout, heap);

	if (testblock->capacity.bytes == 1765) {
		printf("Test n2 failed: capacity was not changed\n");
		return false;
	}
	if (!(testblock->is_free)) {
		printf("Test n2 failed: block is not free\n");
		return false;
	}

	void* second_test_block = _malloc(1, heap); // due to BLOCK_MIN_CAPACITY it will be 24
	testblock = block_get_header(second_test_block);
	printf("\n\nSecond try\n\n");
	printf("Before freeing:\n");
  debug_heap(stdout, heap);
  _free(second_test_block);
	printf("After freeing:\n");
	debug_heap(stdout, heap);

	if (testblock->capacity.bytes == 24) { // we malloced size of 1 but since BLOCK_MIN_CAPACITY is 24 we're checking for 24
		printf(RED "Test n2 failed: the capacity was not changed\n" RESET);
		return false;
	}
	if (!(testblock->is_free)) {
		printf(RED "Test n2 failed: block is not free\n" RESET);
		return false;
	}

	if (second_test_block != test_block) {
		printf(RED "Test n2 failed: freeing gone wrong\n" RESET);
		return false;
	}
  _free(second_test_block);
  _free(test_block);
	printf(GRN "\n[ TEST n2 successfully passed ]\n\n" RESET);
	return true;
}
