#include "test.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../mem.h"
#include "../mem_internals.h"

// Test of a simple usage
bool test1() {
  printf(BLU " TEST n1 (simple usage)\n" RESET);
	void *heap = heap_init(1000);
	void* test_allocation = _malloc(2002, heap);
  debug_heap(stdout, heap);
	if (test_allocation == NULL) {
		printf(RED "Test n1 failed: allocation was unsuccessful\n" RESET);
		return false;
	}

	struct block_header *block = block_get_header(test_allocation);
	if (block->capacity.bytes != 2002) {
		  printf(RED "Test n1 failed: the capacity was not matching the needed capacity\n" RESET);
		  return false;
	}

	_free(test_allocation);
  debug_heap(stdout, heap);
	printf(GRN "\n\n[ TEST n1 successfully passed ]\n\n" RESET);
	return true;
}
