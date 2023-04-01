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
    arena->alloc_list = NULL;

    return arena;
}

void dealloc_arena(arena_t *arena)
{
    // ll_free(arena->alloc_list);

    // //free(arena->alloc_list);
    // free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
    block_t *block = NULL;

	miniblock_t *first_miniblock = NULL;
	first_miniblock->start_address = address;
	first_miniblock->size = size;
	first_miniblock->perm = 6; //TODO ce plm e asta
	first_miniblock->rw_buffer = NULL;

	block_t *neighbor_r = search_alloc(arena, 0, address + size, arena->arena_size);
	block_t *neighbor_l = search_alloc(arena, 0, 0, address - 1);

	if (neighbor_l && neighbor_r) {
		block->start_address = neighbor_l->start_address;
		block->size = neighbor_l->size + size + neighbor_r->size;
		
		size_t new_size = ((list_t *)neighbor_l->miniblock_list)->size 
						+ ((list_t *)neighbor_r->miniblock_list)->size
						+ size;
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
		block->start_address = address;
		block->size = size;
		//block->miniblock_list = malloc(sizeof(miniblock_t));
	
		block->miniblock_list = ll_create(sizeof(miniblock_t));
		ll_add_nth_node(block->miniblock_list, 0, first_miniblock);
		
		ll_add_nth_node(arena->alloc_list, arena->alloc_list->size, &block);
	}
}

void free_block(arena_t *arena, const uint64_t address)
{

}

void read(arena_t *arena, uint64_t address, uint64_t size)
{

}

void write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data)
{

}

void pmap(const arena_t *arena)
{

}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{

}

block_t *search_alloc(arena_t *arena, const short way, 
					  const uint64_t start, const uint64_t last)
{
	/* find check if an area has any part of it unavailable*/
	/*return NULL if no blocks, return it if found*/
	if (way == 0) {//daca nu conteaza
		node_t *node = arena->alloc_list->head;
		while (node) {
			if (((block_t *)node->data)->start_address >= start &&
				((block_t *)node->data)->start_address <= last) {
				return (block_t *)node->data;//pulamea
			}
			
			uint64_t arena_last = ((block_t *)node->data)->start_address + ((block_t *)node->data)->size - 1;
			if (arena_last <= last && arena_last >= start) {
				return (block_t *)node->data;//pulamea
			}
			node = node->next;
		}
		
	} //else if (way == 1) { //daca cautam doar la dreapta

	// } else if (way == 2) { //daca cautam doar la stanga

	// }

	return NULL;
}
