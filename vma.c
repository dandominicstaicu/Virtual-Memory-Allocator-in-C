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

void dealloc_arena(arena_t *arena)
{
    uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;
		free_block(arena, block->start_address);
		block_list = block_list->next;
	}

	free(arena->alloc_list);
	free(arena);

	exit(0);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
    block_t *block = (block_t *)malloc(sizeof(block_t));
	if (!block) {
		fprintf(stderr, "block alloc failed");
		exit(-1);
	}

	miniblock_t *first_miniblock = (miniblock_t *)malloc(sizeof(miniblock_t));
	if (!first_miniblock) {
		fprintf(stderr, "malloc failed in alloc at first_minibloc\n");
		return;
	}

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
		//printf("case both\n");
		block->size = neighbor_l->size + size + neighbor_r->size;
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


		uint64_t addr =  neighbor_l->start_address;
		
		//HERE WE NEED FREE OF BOTH NEIGH L AND R BECAUSE WE CANT DO BOTH SAME TIME

		uint64_t cnt_mini_r = ((list_t *)neighbor_r->miniblock_list)->size;
		node_t *mini_list_r = ((list_t *)neighbor_r->miniblock_list)->head;

		for (uint64_t j = 0; j < cnt_mini_r; ++j) {
			miniblock_t *old_mini_r = (miniblock_t *)mini_list_r->data;
			free_block(arena, old_mini_r->start_address);

			mini_list_r = mini_list_r->next;
		}

		// TODO free neighbor !!!!

		uint64_t cnt_mini_l = ((list_t *)neighbor_l->miniblock_list)->size;
		node_t *mini_list_l = ((list_t *)neighbor_l->miniblock_list)->head;

		for (uint64_t j = 0; j < cnt_mini_l; ++j) {
			miniblock_t *old_mini_l = (miniblock_t *)mini_list_l->data;
			free_block(arena, old_mini_l->start_address);

			mini_list_l = mini_list_l->next;
		}

		//remove from arena list the left neighbor with ALL ITS MINIBLOCKS NOT ONLY ONE
		//uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		//node_t *old_miniblock_list = ((list_t *)block->miniblock_list)->head;

		//printf("cnt pulamea: %lu\n", cnt_miniblock);

		//here above is the problem

		// for (uint64_t j = 0; j < cnt_miniblock; ++j) {
		// 	miniblock_t *old_miniblock = (miniblock_t *)old_miniblock_list->data;
		// 	free_block(arena, old_miniblock->start_address);

		// 	old_miniblock_list = old_miniblock_list->next;
		// }
		
		/*REMAKE ASAP???*/
		block->start_address = addr;
		
	} else if (neighbor_r/*imd la dreapta, in continuare*/) {
		block->size = size + neighbor_r->size;
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));


		ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, first_miniblock);
	
		list_t *right_list = (list_t *)neighbor_r->miniblock_list;
		node_t *node = right_list->head;
		for (unsigned int i = 0; i < right_list->size; ++i) {
			ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, node->data);
			node = node->next;
		}
		
		//remove from the arena list right neighbor
		free_block(arena, neighbor_r->start_address);

		block->start_address = address;
		//adaugam blocul nou mai mare in lista de blocuri
		
	} else if (neighbor_l/*imd in stanga, inainte*/) {
		block->size = neighbor_l->size + size;
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));

		list_t *left_list = (list_t *)neighbor_l->miniblock_list;
		node_t *node = left_list->head;
		for (unsigned int i = 0; i < left_list->size; ++i) {
			ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, node->data);
			node = node->next;
			
		}

		ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, first_miniblock);		


		uint64_t addr =  neighbor_l->start_address;
		
		//remove from arena list the left neighbor with ALL ITS MINIBLOCKS NOT ONLY ONE
		uint64_t cnt_miniblock = ((list_t *)neighbor_l->miniblock_list)->size;
		node_t *old_miniblock_list = ((list_t *)neighbor_l->miniblock_list)->head;

		for (uint64_t j = 0; j < cnt_miniblock; ++j) {
			miniblock_t *old_miniblock = (miniblock_t *)old_miniblock_list->data;
			free_block(arena, old_miniblock->start_address);

			old_miniblock_list = old_miniblock_list->next;
		}
		
		/*REMAKE ASAP???*/
		block->start_address = addr;
		
	} else {
        //printf("alocare fara lipire\n");

		block->start_address = address;
		block->size = (size_t)size;
		block->miniblock_list = (list_t *)ll_create(sizeof(miniblock_t));
		
        ll_add_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size, first_miniblock);
		
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
		in_list_block = (block_t *)block_list->data;
	}
	
	ll_add_nth_node(arena->alloc_list, pos, block);

}

void free_block(arena_t *arena, const uint64_t address)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	char found = 0;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		for (uint64_t j = 0; j < cnt_miniblock; ++j) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;

			uint64_t start_mini = miniblock->start_address;
			//uint64_t end_block = block->start_address + block->size; sa ne pisam pe el

			
			if (start_mini == address) {
				found = 1;

				//if the miniblock is the only one in the block
				if (miniblock->size == block->size) {

					ll_free((list_t **)&block->miniblock_list); //God help me

					block_list = block_list->next;
					node_t *block_rmv = ll_remove_nth_node((list_t *)arena->alloc_list, i);
					free(block_rmv->data);
					free(block_rmv);

					return;
				}

				// if there are more miniblocks inside the current block
				if (start_mini == block->start_address) { /*mini la inceput*/
					block->start_address = block->start_address + miniblock->size;
					block->size -= miniblock->size;

					node_t *mini_rmv = ll_remove_nth_node((list_t *)block->miniblock_list, 0);
					free(mini_rmv->data);
					free(mini_rmv);

				} else if (start_mini == block->start_address + block->size - miniblock->size) { /*mini la sfarsit*/
					block->size -= miniblock->size;

					node_t *mini_rmv = ll_remove_nth_node((list_t *)block->miniblock_list, ((list_t *)block->miniblock_list)->size);
					free(mini_rmv->data);
					free(mini_rmv);
				} else {/*la mijloc*/
					//create new blocks with what was on left and right
					node_t *loc_mini_list = ((list_t *)block->miniblock_list)->head;

					node_t *blck_rmv = ll_remove_nth_node((list_t *)arena->alloc_list, i);
					//node_t *blck_rmv = ll_remove_nth_node((list_t *)arena->alloc_list, i);
					
					for (uint64_t k = 0; k < cnt_miniblock; ++k) {
						miniblock_t *loc_mini = (miniblock_t *)loc_mini_list->data;

						if (loc_mini->start_address != address) {
							alloc_block(arena, loc_mini->start_address, loc_mini->size);
						}
						loc_mini_list = loc_mini_list->next;
					}

					//delete and free the whole old block
					ll_free((list_t **)&block->miniblock_list); //God help me

					block_list = block_list->next;
					
					free(blck_rmv->data);
					free(blck_rmv);

					return;
				}
			}

			miniblock_list = miniblock_list->next;
		}

		block_list = block_list->next;
	}

	if (!found) {
		error_inv_addr_free();
		return;
	}
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		//uint64_t start_block = block->start_address;
		uint64_t end_block = block->start_address + block->size -1;
	
		if (address + size - 1 > end_block) {
			uint64_t available_space = end_block - address + 1;
			warn_read(available_space);
		}
		print_from_miniblock(block);
		block_list = block_list->next;
	}
}

void write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t start_block = block->start_address;
		uint64_t end_block = block->start_address + block->size -1;

		if (address >= start_block && address <= end_block) {
			//printf("found start addr\n");
			
			if (address + size - 1 <= end_block) {
				//copy in miniblockuri la data
				copy_to_miniblock(block, data);
			} else {
				uint64_t available_space = end_block - address + 1;
				int8_t *new_data = malloc(sizeof(int8_t) * available_space);
				if (!new_data) {
					fprintf(stderr, "could not alloc new data\n");
					exit(-1);
				}

				memcpy(new_data, data, available_space);
				
				
				//copy in miniblockuri la new_data
				uint8_t succes = copy_to_miniblock(block, new_data);

				if (succes) 
					warn_write(available_space);

			}
		}
		block_list = block_list->next;
	}
}


void pmap(const arena_t *arena)
{
    uint64_t cnt_block = arena->alloc_list->size;
	uint64_t free_size = arena->arena_size;
	unsigned int all_miniblocks = 0;

	node_t *block_list = (node_t *)arena->alloc_list->head;

	for (uint64_t i = 1; i <= cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;
		free_size -= block->size;

		all_miniblocks += dll_get_size((list_t *)block->miniblock_list);

		block_list = block_list->next;
	}

	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	printf("Free memory: 0x%lX bytes\n", free_size);
	printf("Number of allocated blocks: %ld\n", cnt_block);
	printf("Number of allocated miniblocks: %d\n", all_miniblocks);

	for (uint64_t i = 1; i <= cnt_block; ++i) {
		printf("\n");
		block_t *block = (block_t *)block_list->data;
		printf("Block %ld begin\n", i);
		//TODO here X
		printf("Zone: 0x%lX - 0x%lX\n", block->start_address, block->start_address + block->size);
	//TODO not sure about zone line idk +/- 1 idk idk idk man i go crazy
	//nvm this is good :)

		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		for (uint64_t j = 1; j <= cnt_miniblock; ++j) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;
			printf("Miniblock %ld:", j);
			printf("\t\t0x%lX\t\t-\t\t0x%lX\t\t| ", miniblock->start_address, miniblock->start_address + miniblock->size);
			//TODO not sure about zone line idk +/- 1 idk idk idk man i go crazy

			//RW-\n

			print_perm(miniblock->perm);

			miniblock_list = miniblock_list->next;
		}

		printf("Block %ld end\n", i);
		block_list = block_list->next;
	}

}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
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
			
			if (start_mini == address) {
				miniblock->perm = num_perm(permission);
				return;
			}

			miniblock_list = miniblock_list->next;
		}

		block_list = block_list->next;
	}

	error_inv_mprot();
}

uint8_t num_perm(int8_t *permission)
{
	uint8_t perm = 0;

	if (strstr((char *)permission, "PROT_NONE"))
		return 0;

	if (strstr((char *)permission, "PROT_READ"))
		perm += 4;

	if (strstr((char *)permission, "PROT_WRITE"))
		perm += 2;

	if (strstr((char *)permission, "PROT_EXEC"))
		perm += 1;

	return perm;
}

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

		if (start >= block->start_address &&
			start <= block->start_address + block->size - 1) {
			return block;
		}

		if (last >= block->start_address &&
			last <= block->start_address + block->size - 1) {
			return block;
		}

		block_list = block_list->next;
	}

	return NULL;
}

void print_from_miniblock(block_t *block)
{
	uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
	node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

	uint64_t offset = 0;

	for (uint64_t j = 0; j < cnt_miniblock; ++j) {
		miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;

		printf("%s", (char *)miniblock->rw_buffer);
		offset += miniblock->size;

		miniblock_list = miniblock_list->next;
	}
	//printf("\n");
}

uint8_t copy_to_miniblock(block_t *block, int8_t *data)
{
	uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
	node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

	uint64_t offset = 0;
	for (uint64_t j = 0; j < cnt_miniblock; ++j) {
		miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;

		uint8_t perm = miniblock->perm;
		if (perm == 0 || perm == 4 || perm == 1 || perm == 5) {
			error_inv_perm_write();
			return 0;
		}

		memcpy(miniblock->rw_buffer, data + offset, miniblock->size);
		offset += miniblock->size;
		
		if (j == cnt_miniblock - 1) {
			char endl[] = "\n";
			strcat(miniblock->rw_buffer, endl);
			//TODO check this shit idk
		}
		miniblock_list = miniblock_list->next;
	}
	return 1;
}

void print_perm(uint8_t permissions)
{
	switch (permissions) {
	case 0:
		printf("---\n");
		break;
	case 1:
		printf("--X\n");
		break;
	case 2:
		printf("-W-\n");
		break;
	case 3:
		printf("-WX\n");
		break;
	case 4:
		printf("R--\n");
		break;
	case 5:
		printf("R-X\n");
		break;
	case 6:
		printf("RW-\n");
		break;
	case 7:
		printf("RWX\n");
		break;
	default:
		break;
	}
}
