//Copyright 2023 Dan-Dominic Staicu 311CAb
#include "utils.h"

short hash_command(char *command)
{
	if (strcmp(command, "ALLOC_ARENA") == 0)
		return 0;
	else if (strcmp(command, "DEALLOC_ARENA") == 0)
		return 1;
	else if (strcmp(command, "ALLOC_BLOCK") == 0)
		return 2;
	else if (strcmp(command, "FREE_BLOCK") == 0)
		return 3;
	else if (strcmp(command, "READ") == 0)
		return 4;
	else if (strcmp(command, "WRITE") == 0)
		return 5;
	else if (strcmp(command, "PMAP") == 0)
		return 6;
	else if (strcmp(command, "MPROTECT") == 0)
		return 7;

	return -1;
}

block_t *search_alloc(arena_t *arena, const uint64_t start, const uint64_t last)
{
	node_t *block_list = arena->alloc_list->head;
	uint64_t cnt_block = arena->alloc_list->size;

	//check if inside [start;last] is any part of any block/miniblock alloc'd
	for (uint64_t i = 1; i <= cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		if (block->start_address >= start &&
			block->start_address <= last) {
			return block;
		}

		uint64_t arena_last = block->start_address + block->size - 1;
		if (arena_last <= last && arena_last >= start)
			return block;

		if (start >= block->start_address &&
			start <= block->start_address + block->size - 1)
			return block;

		if (last >= block->start_address &&
			last <= block->start_address + block->size - 1)
			return block;

		block_list = block_list->next;
	}

	return NULL;
}
