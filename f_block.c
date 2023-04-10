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

	free_block(arena, block_address, 1);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	block_t *block = malloc(sizeof(block_t));
	if (!block) {
		fprintf(stderr, "block alloc failed");
		exit(-1);
	}

	miniblock_t *first_miniblock = malloc(sizeof(miniblock_t));
	if (!first_miniblock) {
		fprintf(stderr, "malloc failed in alloc at first_minibloc\n");
		free(block);
		return;
	}

	first_miniblock->start_address = address;
	first_miniblock->size = size;
	first_miniblock->perm = 6;

	first_miniblock->rw_buffer = calloc(size + 2, sizeof(int8_t));
	if (!first_miniblock->rw_buffer) {
		fprintf(stderr, "rw_buffer calloc failed");
		free(block);
		free(first_miniblock);
		exit(-1);
	}

	block_t *neighbor_r = search_alloc(arena, address + size, address + size);
	block_t *neighbor_l = search_alloc(arena, address - 1, address - 1);

	//both neighbors exist, before and after the new block
	if (neighbor_l && neighbor_r) {
		both_neighbors(block, neighbor_l, neighbor_r,
					   first_miniblock, arena, size);

	} else if (neighbor_r) { //neighbor only on right, next to the new one
		just_right(block, neighbor_r, arena, first_miniblock, size, address);

	} else if (neighbor_l) { //neighbor only on left, before to the new one
		just_left(block, neighbor_l, arena, first_miniblock, size);

	} else { //no neighbors
		block->start_address = address;
		block->size = (size_t)size;
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));

		list_t *mini_list = (list_t *)block->miniblock_list;

		ll_add_nth_node(mini_list, mini_list->size, first_miniblock);
	}

	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;
	unsigned int pos = 0;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *in_list_block = (block_t *)block_list->data;
		if (block->start_address < in_list_block->start_address)
			break;

		++pos;
		block_list = block_list->next;
	}

	ll_add_nth_node(arena->alloc_list, pos, block);
	free(block);
	free(first_miniblock);
}

void just_left(block_t *block, block_t *neighbor_l, arena_t *arena,
			   miniblock_t *first_miniblock, uint64_t size)
{
	block->size = neighbor_l->size + size;
	block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));

	list_t *left_list = (list_t *)neighbor_l->miniblock_list;
	node_t *node = left_list->head;

	list_t *mini_list = NULL;

	for (unsigned int i = 0; i < left_list->size; ++i) {
		mini_list = (list_t *)block->miniblock_list;
		ll_add_nth_node(mini_list, mini_list->size, node->data);
		node = node->next;
	}

	mini_list = (list_t *)block->miniblock_list;
	ll_add_nth_node(mini_list, mini_list->size, first_miniblock);

	uint64_t addr =  neighbor_l->start_address;

	//remove from arena list the left neighbor with ALL its miniblocks
	uint64_t cnt_miniblock = left_list->size;
	node_t *old_miniblock_l = left_list->head;

	for (uint64_t j = 0; j < cnt_miniblock; ++j) {
		miniblock_t *old_miniblock = (miniblock_t *)old_miniblock_l->data;

		old_miniblock_l = old_miniblock_l->next;

		uint64_t old_addr = old_miniblock->start_address;
		free_block(arena, old_addr, 0);
	}

	block->start_address = addr;
}

void just_right(block_t *block, block_t *neighbor_r, arena_t *arena,
				miniblock_t *first_miniblock, uint64_t size, uint64_t address)
{
	block->size = size + neighbor_r->size;
	block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));

	list_t *mini_list = (list_t *)block->miniblock_list;

	ll_add_nth_node(mini_list, mini_list->size, first_miniblock);

	list_t *right_list = (list_t *)neighbor_r->miniblock_list;
	node_t *node = right_list->head;
	for (unsigned int i = 0; i < right_list->size; ++i) {
		mini_list = (list_t *)block->miniblock_list;
		ll_add_nth_node(mini_list, mini_list->size, node->data);
		node = node->next;
	}

	//remove from the arena list right neighbor
	free_block(arena, neighbor_r->start_address, 0);

	block->start_address = address;
}

void both_neighbors(block_t *block, block_t *neighbor_l, block_t *neighbor_r,
					miniblock_t *first_miniblock, arena_t *arena,
					uint64_t size)
{
	block->size = neighbor_l->size + size + neighbor_r->size;
	block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));

	list_t *left_list = (list_t *)neighbor_l->miniblock_list;
	node_t *node = left_list->head;

	list_t *mini_list = NULL;

	for (unsigned int i = 0; i < left_list->size; ++i) {
		mini_list = (list_t *)block->miniblock_list;
		ll_add_nth_node(mini_list, mini_list->size, node->data);
		node = node->next;
	}

	mini_list = (list_t *)block->miniblock_list;

	ll_add_nth_node(mini_list, mini_list->size, first_miniblock);

	list_t *right_list = (list_t *)neighbor_r->miniblock_list;
	node = right_list->head;
	for (unsigned int i = 0; i < right_list->size; ++i) {
		mini_list = (list_t *)block->miniblock_list;
		ll_add_nth_node(mini_list, mini_list->size, node->data);
		node = node->next;
	}

	uint64_t addr =  neighbor_l->start_address;

	uint64_t cnt_mini_r = ((list_t *)neighbor_r->miniblock_list)->size;
	node_t *mini_list_r = ((list_t *)neighbor_r->miniblock_list)->head;

	for (uint64_t j = 0; j < cnt_mini_r; ++j) {
		miniblock_t *old_mini_r = (miniblock_t *)mini_list_r->data;

		mini_list_r = mini_list_r->next;

		uint64_t addr = old_mini_r->start_address;
		free_block(arena, addr, 0);
	}

	uint64_t cnt_mini_l = ((list_t *)neighbor_l->miniblock_list)->size;
	node_t *mini_list_l = ((list_t *)neighbor_l->miniblock_list)->head;

	for (uint64_t j = 0; j < cnt_mini_l; ++j) {
		miniblock_t *old_mini_l = (miniblock_t *)mini_list_l->data;

		mini_list_l = mini_list_l->next;

		uint64_t addr = old_mini_l->start_address;
		free_block(arena, addr, 0);
	}

	block->start_address = addr;
}

void free_block(arena_t *arena, const uint64_t address, uint64_t final)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		for (uint64_t j = 0; j < cnt_miniblock; ++j) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;

			uint64_t start_mini = miniblock->start_address;
			uint64_t end = block->start_address + block->size - miniblock->size;

			if (start_mini == address) {
				if (final == 1)
					free(miniblock->rw_buffer);

				//if the miniblock is the only one in the block
				if (miniblock->size == block->size) {
					ll_free((list_t **)&block->miniblock_list); //God help me
					list_t *alloc_l = (list_t *)arena->alloc_list;

					node_t *block_rmv = ll_remove_nth_node(alloc_l, i);
					free(block_rmv->data);
					free(block_rmv);
					return;
				} else if (start_mini == block->start_address) {
					//miniblock at begining
					// if there are more miniblocks inside the current block
					block->start_address += miniblock->size;
					block->size -= miniblock->size;

					list_t *mini_list = (list_t *)block->miniblock_list;
					node_t *mini_rmv = ll_remove_nth_node(mini_list, 0);
					free(mini_rmv->data);
					free(mini_rmv);
					return;
				} else if (start_mini == end) {
					// miniblock at the end
					block->size -= miniblock->size;

					list_t *mini_list = (list_t *)block->miniblock_list;
					node_t *mini_rmv = ll_remove_nth_node(mini_list,
														  mini_list->size);
					free(mini_rmv->data);
					free(mini_rmv);

					return;
				}
				// else it means it's in middle
				//create new blocks with what was on left and right
				node_t *loc_mini_list = ((list_t *)block->miniblock_list)->head;
				list_t *aloc_l = (list_t *)arena->alloc_list;
				node_t *blck_rmv = ll_remove_nth_node(aloc_l, i);

				for (uint64_t k = 0; k < cnt_miniblock; ++k) {
					miniblock_t *loc_mini = (miniblock_t *)loc_mini_list->data;

					if (loc_mini->start_address != address) {
						free(loc_mini->rw_buffer);
						alloc_block(arena, loc_mini->start_address,
									loc_mini->size);
					}

					loc_mini_list = loc_mini_list->next;
				}
				//delete and free the whole old block
				ll_free((list_t **)&block->miniblock_list);
				free(blck_rmv->data);
				free(blck_rmv);
				return;
			}
			miniblock_list = miniblock_list->next;
		}
		block_list = block_list->next;
	}
	error_inv_addr_free();
}
