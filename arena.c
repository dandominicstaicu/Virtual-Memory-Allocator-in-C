#include "arena.h"

void f_arena(void)
{
	uint64_t arena_size;
	scanf("%ld", &arena_size);

	arena_t *arena = alloc_arena(arena_size);
	if (!arena) {
		fprintf(stderr, "failed\n");
		exit(-1);
	}

	
}