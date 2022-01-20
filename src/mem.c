#define _DEFAULT_SOURCE
#include "mem_internals.h"
#include "mem.h"
#include "util.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>



void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

extern inline block_size size_from_capacity( block_capacity cap );
extern inline block_capacity capacity_from_size( block_size sz );

static bool block_is_big_enough(size_t query, struct block_header* block) { 
	return block->capacity.bytes >= query; 
}
static size_t pages_count(size_t mem) { 
	return mem / getpagesize() + ((mem % getpagesize()) > 0); 
}
static size_t round_pages(size_t mem) { 
	return getpagesize() * pages_count( mem ) ; 
}

static void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

static size_t region_actual_size(size_t query) { 
	return size_max( round_pages( query ), REGION_MIN_SIZE ); 
}

extern inline bool region_is_invalid(const struct region* r);


static void* map_pages(void const* addr, size_t length, int additional_flags) {
  return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , -1, 0 );
}

/*  аллоцировать регион памяти и инициализировать его блоком */
static struct region alloc_region  ( void const * addr, size_t query ) {
	query = region_actual_size(query);
	void* region_addr = map_pages(addr, query, MAP_FIXED_NOREPLACE);
  if (errno == EEXIST) {
    region_addr = map_pages(addr, query, 0);
  }
  if (region_addr == MAP_FAILED) return REGION_INVALID;
  struct region ret = {
    .addr = region_addr,
    .size = query,
    .extends = false
  };
  if (!region_is_invalid(&ret)) {
    block_size size = {query};
    block_init(ret.addr, size, NULL);  
  }
  return ret;
}

static void* block_after( struct block_header const* block )         ;

void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial );
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

static bool split_if_too_big( struct block_header* block, size_t query ) {
	if (!block_splittable(block, query)) {
		//printf("split false\n");
		return false;
	}
	else {
		//printf("split true\n");
		if (query < BLOCK_MIN_CAPACITY) {
			query = BLOCK_MIN_CAPACITY;
		}
		block_size free_new_block_size = {block->capacity.bytes - query};
		block->capacity.bytes = query; 

		void* new_block_addr = block_after(block);  // Адрес нового (второго) блока
		//block_size free_new_block_size = {block->capacity.bytes - query};

		// Первый блок размером по запросу, второй оставшийся размер. 
		block_init(new_block_addr, free_new_block_size, NULL); // Инициализируем второй блок
		block->next = new_block_addr; // Первому блоку даем адрес второго блока
		return true;
	}
}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block ) {
	return  (void*) (block->contents + block->capacity.bytes);
}
static bool blocks_continuous (
                               struct block_header const* fst,
                               struct block_header const* snd ) {
  return (void*)snd == block_after(fst);
}

static bool mergeable(struct block_header const* restrict fst, struct block_header const* restrict snd) {
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

static bool try_merge_with_next( struct block_header* block ) {
		if ((block->next) && (mergeable(block,block->next))) {
	    //block->capacity.bytes = block->capacity.bytes + block->next->capacity.bytes;
			block->capacity.bytes += size_from_capacity(block->next->capacity).bytes;
	    block->next = block->next->next;
	    return true;
		}
		else {
			return false; // очевидно, что если нет следующего блока или мердж невозможен, то false
		}
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};


static struct block_search_result find_good_or_last  ( struct block_header* restrict block, size_t sz )    {
	struct block_header* block_behind = NULL;	
	
	while (block != NULL) {
		while (try_merge_with_next(block));
		if ((block_is_big_enough(sz, block)) && block->is_free) {
			return (struct block_search_result) { 
				.type = BSR_FOUND_GOOD_BLOCK, 
				.block = block 
			};
		}

		block_behind = block;
		block = block->next;
	}

	return (struct block_search_result) { 
		//.type = BSR_CORRUPTED, 
		.type = BSR_REACHED_END_NOT_FOUND,
		.block = block_behind
	};

}

/*  Попробовать выделить память в куче начиная с блка `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {
	return find_good_or_last(block, query);
}



static struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
  if (query < BLOCK_MIN_CAPACITY) {
		query = BLOCK_MIN_CAPACITY;
	}

	struct region grew_region = alloc_region(block_after(last), query);
	last->next = grew_region.addr;
	if (try_merge_with_next(last)) {
		return last;
	}  
	return last->next;
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header* memalloc( size_t query, struct block_header* heap_start) {
	
	if (query < BLOCK_MIN_CAPACITY) { 
		query = BLOCK_MIN_CAPACITY;
	}
	struct block_search_result result = try_memalloc_existing(query, heap_start);
  switch (result.type) {
    case BSR_FOUND_GOOD_BLOCK:
			//printf("BSR_FOUND_GOOD_BLOCK CASE\n");
			split_if_too_big(result.block, query);
      result.block->is_free = false;
      return result.block;
    case BSR_REACHED_END_NOT_FOUND:
			//printf("BSR_REACHED_END_NOT_FOUND CASE\n");
      grow_heap(result.block, query);
      result = try_memalloc_existing(query, heap_start);
			if (result.type != BSR_FOUND_GOOD_BLOCK) {
				return NULL;
			}
			else {
      	result.block->is_free = false;
      	return result.block;
			}
    default:
      return NULL;
	}
}

void* _malloc(size_t query, void* heap_start) {
  struct block_header* const addr = memalloc( query, (struct block_header*) heap_start );
  if (addr) return addr->contents;
  else return NULL;
}

struct block_header *block_get_header(void *contents) {
    return (struct block_header *) (((uint8_t *) contents) - offsetof(struct block_header, contents));
}


void _free(void *mem) {
    if (!mem) {
			return;
		}
    struct block_header* header = block_get_header(mem);
    header->is_free = true;
		while (try_merge_with_next(header));
}

