#include "arena.h"

void f_alloc_arena(arena_t **arena)
{
	uint64_t arena_size;
	scanf("%lu", &arena_size);

	*arena = alloc_arena(arena_size);
	if (!(*arena)) {
		fprintf(stderr, "failed alloc in f_alloc\n");
		exit(-1);
	}

	(*arena)->arena_size = arena_size;
	(*arena)->alloc_list = ll_create(sizeof(block_t));
}

void f_dealloc_arena(void)
{

}
