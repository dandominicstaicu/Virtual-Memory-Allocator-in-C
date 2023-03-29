#include "arena.h"

void f_alloc_arena(arena_t **arena)
{
	uint64_t arena_size;
	scanf("%lu", &arena_size);

	*arena = alloc_arena(arena_size);
	if (!(*arena)) {
		fprintf(stderr, "failed\n");
		exit(-1);
	}
}

void f_dealloc_arena(void)
{

}
