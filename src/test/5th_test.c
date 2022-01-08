#include "test.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "../mem.h"
#include "../mem_internals.h"

// End of the memory, the old memory region can't be extended because of a different allocated address range.
// Allocating the new region somewhere else.


bool test5() {
	printf(BLU "\n\n TEST n5 (allocating the new region somewhere else due to a different allocated address range) \n\n" RESET);
	void* first_heap = heap_init(5000);

  debug_heap(stdout, first_heap);
  void* first_test_block = _malloc(3000, first_heap);
  struct block_header *firsttestblock = (first_test_block);

  void* first_testblock = firsttestblock + 9900;

  void* mmaped = mmap(first_testblock, 1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if (!mmaped) {
		printf(RED "Test n5 failed: wrong mmaping\n" RESET);	
		return false;
	}

  debug_heap(stdout, first_heap);

	_malloc(5000, first_heap);
	debug_heap(stdout, first_heap);

	printf(GRN "\n[ TEST n5 successfully passed ]\n\n" RESET);
	return true;
}
