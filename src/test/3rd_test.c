#include "test.h"
#include "../mem.h"
#include "../mem_internals.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



extern void* heap;

// Free 2 blocks from multiple blocks
bool test3() {
	printf(BLU "\n\n TEST n3 (freeing 2 block from 3 blocks) \n\n" RESET);

	size_t first_block_size = 1765; // the start of the American Revolution 
	size_t second_block_size = 1791; // the end of the American Revolution
	void *test1_block = _malloc(first_block_size, heap); // the start of the American Revolution
	void *test2_block = _malloc(second_block_size, heap); // the end of the American Revolution


	struct block_header *testblock1 = block_get_header(test1_block);
	struct block_header *testblock2 = block_get_header(test2_block);
	printf("Before freeing:\n");
  debug_heap(stdout, heap);
  _free(test2_block);
	_free(test1_block);
	printf("After freeing:\n");
  debug_heap(stdout, heap);
	//printf("printf]: testblock 1 capacity: %ld\n",testblock1->capacity.bytes);
	//printf("printf]: testblock 1 free?: %d\n",testblock1->is_free);
	if (testblock1->capacity.bytes == first_block_size) {
		printf(RED "Test n3 failed: the capacity of block 1 was not changed\n" RESET);
		return false;
	}
	if (!(testblock1->is_free)) {
		printf(RED "Test n3 failed: the block 1 was not free\n" RESET);
		return false;
	}
	//printf("printf]: testblock 2 capacity: %ld\n",testblock2->capacity.bytes);
	//printf("printf]: testblock 2 free?: %d\n",testblock2->is_free);
	if (testblock2->capacity.bytes == second_block_size) {
		printf(RED "Test n3 failed: the capacity of block 2 was not changed\n" RESET);
		return false;
	}
	if (!(testblock2->is_free)) {
		printf(RED "Test n3 failed: the block 2 was not free\n" RESET);
		return false;
	}
	printf(GRN "\n[ TEST n3 successfully passed ]\n\n" RESET);
	_free(test1_block);
	_free(test2_block);
	return true;
}
