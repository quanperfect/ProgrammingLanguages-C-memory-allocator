#include "test.h"
#include "../mem.h"
#include "../mem_internals.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>





// End of the memory, the old memory region can't be extended because of a different allocated address range.
// Allocating the new region somewhere else.
extern void* heap;

bool test5() {
	printf(BLU "\n\n TEST n5 (allocating the new region somewhere else due to a different allocated address range) \n\n" RESET);

  debug_heap(stdout, heap);
  void* first_test_block = _malloc(3000, heap);
  struct block_header *firsttestblock = (first_test_block);

  void* first_testblock = firsttestblock + 9900;

  void* mmaped = mmap(first_testblock, 1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if (!mmaped) {
		printf(RED "Test n5 failed: wrong mmaping\n" RESET);	
		return false;
	}

  debug_heap(stdout, heap);

	_malloc(5000, heap);
	debug_heap(stdout, heap);

	printf(GRN "\n[ TEST n5 successfully passed ]\n\n" RESET);
	return true;
}
