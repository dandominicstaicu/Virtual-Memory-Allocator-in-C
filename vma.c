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

	miniblock_t *first_miniblock = (miniblock_t *)malloc(sizeof(miniblock_t));
	first_miniblock->start_address = address;
	first_miniblock->size = size;
	first_miniblock->perm = 6; //TODO ce plm e asta
	first_miniblock->rw_buffer = malloc(sizeof(char) * size);

	block_t *neighbor_r = search_alloc(arena, address + size, address + size + 1);
	block_t *neighbor_l = search_alloc(arena, address - 2, address - 1);



	if (neighbor_l && neighbor_r) {
		block->start_address = neighbor_l->start_address;
		block->size = neighbor_l->size + size + neighbor_r->size;
		
		// size_t new_size = ((list_t *)neighbor_l->miniblock_list)->size 
		// 				+ ((list_t *)neighbor_r->miniblock_list)->size
		// 				+ size;
		//block->miniblock_list = malloc(new_size);

		block->miniblock_list = ll_create(sizeof(miniblock_t));

		node_t *node = ((list_t *)neighbor_l->miniblock_list)->head;
		while (node) {
			ll_add_nth_node(block->miniblock_list, block->size, node->data);
			node = node->next;
		}

		ll_add_nth_node(block->miniblock_list, block->size, first_miniblock);

		node = ((list_t *)neighbor_r->miniblock_list)->head;
		while (node) {
			ll_add_nth_node(block->miniblock_list, block->size, node->data);
			node = node->next;
		}
		
		//adaugam blocul nou mai mare in lista de blocuri
		ll_add_nth_node(arena->alloc_list, arena->arena_size, &block);

		//scoatem din lista de blocuri din arena vecinii
		//free_block_imp(neighbor_l);
		//free_block_imp(neighbor_r);

	} else if (neighbor_r/*imd la dreapta, in continuare*/) {
		block->start_address = address;
		block->size = size + neighbor_r->size;

		block->miniblock_list = ll_create(sizeof(miniblock_t));

		ll_add_nth_node(block->miniblock_list, 0, &first_miniblock);
	
		node_t *node = ((list_t *)neighbor_r->miniblock_list)->head;
		while (node) {
			ll_add_nth_node(block->miniblock_list, block->size, node->data);
			node = node->next;
		}

		ll_add_nth_node(arena->alloc_list, arena->arena_size, &block);

		//scoatem din arena pe vecinu din right
	
	} else if (neighbor_l/*imd in stanga, inainte*/) {
		block->start_address = neighbor_l->start_address;
		block->size = neighbor_l->size + size;

		block->miniblock_list = ll_create(sizeof(miniblock_t));

		node_t *node = ((list_t *)neighbor_l->miniblock_list)->head;
		while (node) {
			ll_add_nth_node(block->miniblock_list, block->size, node->data);
			node = node->next;
		}

		ll_add_nth_node(block->miniblock_list, block->size, &first_miniblock);
		
		ll_add_nth_node(arena->alloc_list, arena->arena_size, &block);

		//scoatem din lista arenei pe vecinu din left
	} else {
        printf("alocare buna\n");
		
		block->start_address = address;
		//memcpy(block->start_address, address, sizeof(uint64_t));
		block->size = (size_t)size;
		//memcpy(block->size, size, sizeof(uint64_t));
		//printf("pula pula block size: %ld\n", block->size);
		//block->miniblock_list = malloc(sizeof(miniblock_t));

		//list_t *aux_list = ll_create(sizeof(miniblock_t));
		//memcpy(&block->miniblock_list, &aux_list, sizeof(list_t *));
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

/*
void free_block(arena_t *arena, const uint64_t address)
{

}

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
		
		uint64_t arena_last = block->start_address + block->size;
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
