//Copyright 2023 Dan-Dominic Staicu 311CAb
#include "arena.h"

void f_alloc_arena(arena_t **arena)
{
	uint64_t arena_size = 0;
	scanf("%lu", &arena_size);

	//alloc_arena() returns a pointer to arena_t *
	*arena = alloc_arena(arena_size);
	DIE(!arena, "failed alloc in f_alloc\n");
}

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = malloc(sizeof(arena_t));
	DIE(!arena, "arena malloc failed in alloc_arena\n");

	arena->arena_size = size;
	arena->alloc_list = ll_create(sizeof(block_t));

	return arena;
}

void dealloc_arena(arena_t *arena)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	//iterate all the blocks inside the arena
	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		//skip to the next block inside
		block_list = block_list->next;

		//free the rw_buff of every single miniblock
		//inside a block insinde the arena
		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		// fprintf(stderr, "cnt_minis: %lu\n", cnt_miniblock);

		for (uint64_t j = 0; j < cnt_miniblock; ++j) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;

			free(miniblock->rw_buffer);
			miniblock_list = miniblock_list->next;
		}

		//free the miniblock list of every single block
		ll_free((list_t **)&block->miniblock_list);

		//remove the block from arena's list
		node_t *block_rmv = ll_remove_nth_node((list_t *)arena->alloc_list, 0);
		free(block_rmv->data);
		free(block_rmv);
	}

	//free the memory of arena_t * pointer
	free(arena->alloc_list);
	free(arena);
}
