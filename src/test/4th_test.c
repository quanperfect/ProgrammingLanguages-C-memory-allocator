#include "test.h"
#include "../mem.h"
#include "../mem_internals.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


extern void* heap;
//  printf(RED "red\n"     RESET);
// New regions extends previous region due to running out of memory
bool test4() {
	printf(BLU "\n\n TEST n4 (new region due to running out of memory) \n\n" RESET);

  debug_heap(stdout, heap);
	size_t block_size = 6000; 
	
  void *test1_block = _malloc(block_size, heap);
	if (test1_block == NULL) {
		printf(RED "_malloc test1_block returned null\n" RESET);
		debug_heap(stdout, heap);
		return false;
	}
  void *test2_block = _malloc(block_size, heap);
	if (test2_block == NULL) {
		printf(RED "_malloc test2_block returned null\n" RESET);
	  debug_heap(stdout, heap);
		return false;		
	}

  void *test3_block = _malloc(block_size/2, heap);
	if (test3_block == NULL) {
		printf(RED "_malloc test3_block returned null\n" RESET);
		debug_heap(stdout, heap);
		return false;
	}
	debug_heap(stdout, heap);

  _free(test2_block);
  _free(test3_block);

  debug_heap(stdout, heap);

  struct block_header *test1block = block_get_header(test1_block);
  struct block_header *test2block = block_get_header(test2_block);
  if ((uint8_t *)test1block->contents + test1block->capacity.bytes != (uint8_t*) test2block){
    printf("%p \n", (uint8_t *)test1block->contents + test1block->capacity.bytes);
    printf("%p \n", (uint8_t*) test2block);
    printf(RED "Test n4 failed: the block was not created next to the previous one\n" RESET);
		return false;
  }
	_free(test1_block);
  _free(test2_block);
  _free(test3_block);
	printf(GRN "\n[ TEST n4 successfully passed ]\n\n" RESET);


  return true;

}
