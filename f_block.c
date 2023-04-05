#include "f_block.h"

void f_block(arena_t *arena)
{
	uint64_t arena_address = 0;
	uint64_t block_size = 0;
	scanf("%lu%lu", &arena_address, &block_size);

	if (arena_address >= arena->arena_size) {
		error_inv_alloc_block_out();
		return;
	}

	if (arena_address + block_size > arena->arena_size) {
		error_inv_alloc_block_end();
		return;
	}

	if (search_alloc(arena, arena_address, arena_address + block_size - 1)) {
		error_inv_alloc_block_alr_aloc();
		return;
	}

	alloc_block(arena, arena_address, block_size);
}

void f_free_block(arena_t *arena)
{
	uint64_t block_address = 0;
	scanf("%lu", &block_address);

	if (block_address > arena->arena_size) {
		error_inv_addr_free();
		return;
	}

	free_block(arena, block_address);
}

void f_mprotect(arena_t *arena)
{
	uint64_t arena_address = 0;
	int8_t permissions[50] = "";

	scanf("%lu", &arena_address);
	fgets((char *)permissions, 50, stdin);

	mprotect(arena, arena_address, permissions);
}
