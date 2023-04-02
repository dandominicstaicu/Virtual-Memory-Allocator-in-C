//Copyright 2023 Dan-Dominic Staicu 311CAb
#include "vma.h"

arena_t *alloc_arena(const uint64_t size)
{
    arena_t *arena = malloc(sizeof(arena_t));
    
    if (!arena) {
        fprintf(stderr, "could not alloc mem in alloc_arena\n");
        return NULL;
    }

    arena->arena_size = size;
    arena->alloc_list = ll_create(sizeof(block_t));

    return arena;
}

/*
void dealloc_arena(arena_t *arena)
{
    // ll_free(arena->alloc_list);

    // //free(arena->alloc_list);
    // free(arena);
} */

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
    block_t *block = (block_t *)malloc(sizeof(block_t));
	if (!block) {
		fprintf(stderr, "block alloc failed");
		exit(-1);
	}

	miniblock_t *first_miniblock = (miniblock_t *)malloc(sizeof(miniblock_t));
	//TODO check malloc

	first_miniblock->start_address = address;
	first_miniblock->size = size;
	first_miniblock->perm = 6; //TODO ce plm e asta

	first_miniblock->rw_buffer = malloc(sizeof(char) * size);
	if (!first_miniblock->rw_buffer) {
		fprintf(stderr, "rw_buffer alloc failed");
		exit(-1);
	}

	block_t *neighbor_r = search_alloc(arena, address + size, address + size);
	block_t *neighbor_l = search_alloc(arena, address - 1, address - 1);

	if (neighbor_l && neighbor_r) {
		block->start_address = neighbor_l->start_address;
		block->size = (size_t)(neighbor_l->size + size + neighbor_r->size);
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));

		list_t *left_list = (list_t *)neighbor_l->miniblock_list;
		node_t *node = left_list->head;
		for (unsigned int i = 0; i < left_list->size; ++i) {
			ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, node->data);
			node = node->next;
		}

		ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, first_miniblock);

		list_t *right_list = (list_t *)neighbor_r->miniblock_list;
		node = right_list->head;
		for (unsigned int i = 0; i < right_list->size; ++i) {
			ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, node->data);
			node = node->next;
		}
		
		//adaugam blocul nou mai mare in lista de blocuri
		ll_add_nth_node(arena->alloc_list, arena->alloc_list->size, block);
		
		
		
		//scoatem din lista de blocuri din arena vecinii
		free_block(arena, neighbor_l->start_address);
		free_block(arena, neighbor_r->start_address);

	} else if (neighbor_r/*imd la dreapta, in continuare*/) {
		block->start_address = address;
		block->size = size + neighbor_r->size;
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));


		ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, first_miniblock);
	
		list_t *right_list = (list_t *)neighbor_r->miniblock_list;
		node_t *node = right_list->head;
		for (unsigned int i = 0; i < right_list->size; ++i) {
			ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, node->data);
			node = node->next;
		}
		
		//adaugam blocul nou mai mare in lista de blocuri
		ll_add_nth_node(arena->alloc_list, arena->alloc_list->size, block);
		//scoatem din arena pe vecinu din right
	
	} else if (neighbor_l/*imd in stanga, inainte*/) {
		block->start_address = neighbor_l->start_address;
		block->size = neighbor_l->size + size;
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));

		list_t *left_list = (list_t *)neighbor_l->miniblock_list;
		node_t *node = left_list->head;
		for (unsigned int i = 0; i < left_list->size; ++i) {
			ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, node->data);
			node = node->next;
		}

		ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, first_miniblock);		

		ll_add_nth_node(arena->alloc_list, arena->alloc_list->size, block);

		//scoatem din lista arenei pe vecinu din left
	} else {
        printf("alocare fara lipire\n");
		
		block->start_address = address;
		block->size = (size_t)size;
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));
        
		
        ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, first_miniblock);
		
		ll_add_nth_node(arena->alloc_list, arena->alloc_list->size, block);
    
        //printf("in allocblock: %d", ((list_t *)block->miniblock_list)->size);

		/* DEBUG */
		// int test = ((block_t *)arena->alloc_list->head->data)->size;
		// printf("in functia pulii: %d\n", test);
		//printf("pula: %d",((list_t *)block->miniblock_list)->size);
		//printf("in allocblock: %d", dll_get_size((list_t *)block->miniblock_list));
	}
}

void free_block(arena_t *arena, const uint64_t address)
{

}

/*
void read(arena_t *arena, uint64_t address, uint64_t size)
{

}

void write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data)
{

}

// void pmap(const arena_t *arena)
// {
    
// }

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{

}

*/

block_t *search_alloc(arena_t *arena, const uint64_t start, const uint64_t last)
{
	/* find check if an area has any part of it unavailable*/
	/*return NULL if no blocks, return it if found*/
	//daca nu conteaza
	node_t *block_list = arena->alloc_list->head;
    uint64_t cnt_block = arena->alloc_list->size;

	for (uint64_t i = 1; i <= cnt_block; ++i) {
        block_t *block = (block_t *)block_list->data;

		if (block->start_address >= start &&
			block->start_address <= last) {
			return block;//pulamea
		}
		
		uint64_t arena_last = block->start_address + block->size - 1;
		if (arena_last <= last && arena_last >= start) {
			return block;//pulamea
		}

		block_list = block_list->next;
	}
		
	 //else if (way == 1) { //daca cautam doar la dreapta

	// } else if (way == 2) { //daca cautam doar la stanga

	// }

	return NULL;
}
