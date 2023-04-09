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

		block_list = block_list->next;

		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		for (uint64_t j = 0; j < cnt_miniblock; ++j) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;
			miniblock_list = miniblock_list->next;

			free(miniblock->rw_buffer);
		}

		ll_free((list_t **)&block->miniblock_list);

		node_t *block_rmv = ll_remove_nth_node((list_t *)arena->alloc_list, 0);
		free(block_rmv->data);
		free(block_rmv);
	}

	free(arena->alloc_list);
	free(arena);
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
	} else if (neighbor_r) { //neighbor only on right, next to the new one
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
	} else if (neighbor_l) { //neighbor only on left, before to the new one
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

		list_t *left_list = (list_t *)neighbor_l->miniblock_list;
		uint64_t cnt_miniblock = left_list->size;
		node_t *old_miniblock_l = left_list->head;

		for (uint64_t j = 0; j < cnt_miniblock; ++j) {
			miniblock_t *old_miniblock = (miniblock_t *)old_miniblock_l->data;

			old_miniblock_l = old_miniblock_l->next;

			uint64_t addr = old_miniblock->start_address;
			free_block(arena, addr, 0);
		}

		block->start_address = addr;
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
					node_t *mini_rmv = ll_remove_nth_node(mini_list, mini_list->size);
					free(mini_rmv->data);
					free(mini_rmv);

					return;
				}

				// else it means it's in middle
				//create new blocks with what was on left and right
				node_t *loc_mini_list = ((list_t *)block->miniblock_list)->head;

				node_t *blck_rmv = ll_remove_nth_node((list_t *)arena->alloc_list, i);

				for (uint64_t k = 0; k < cnt_miniblock; ++k) {
					miniblock_t *loc_mini = (miniblock_t *)loc_mini_list->data;

					if (loc_mini->start_address != address) {
						free(loc_mini->rw_buffer);
						alloc_block(arena, loc_mini->start_address, loc_mini->size);
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

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	int8_t found = 0;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t start_block = block->start_address;
		uint64_t end_block = start_block + block->size - 1;

		if (address >= start_block && address <= end_block) {
			uint64_t offset = 0;
			if (address > start_block)
				offset = address - start_block;

			if (address + size - 1 > end_block) {
				uint64_t available_space = end_block - address + 1;
				warn_read(available_space);
			}

			print_from_miniblock(block, address, size, offset);

			found = 1;
		}
		block_list = block_list->next;
	}

	if (!found)
		error_inv_addr_read();
}

void write(arena_t *arena, const uint64_t address,
		   const uint64_t size, int8_t *data)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	uint8_t found = 0;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t start_block = block->start_address;
		uint64_t end_block = block->start_address + block->size - 1;

		if (address >= start_block && address <= end_block) {
			found = 1;

			if (address + size - 1 <= end_block) {
				//copy data in miniblocks
				copy_to_miniblock(block, data);
			} else {
				uint64_t available_space = end_block - address + 1;
				int8_t *new_data = calloc(available_space + 2, sizeof(int8_t));
				if (!new_data) {
					fprintf(stderr, "could not alloc new data\n");
					exit(-1);
				}

				memcpy(new_data, data, available_space);
				memcpy(new_data + available_space + 1, "\0", 1);

				//copy new_data in miniblocks
				uint8_t succes = copy_to_miniblock(block, new_data);

				if (succes)
					warn_write(available_space);

				free(new_data);
			}
		}
		block_list = block_list->next;
	}

	if (!found)
		error_inv_addr_write();
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
		printf("Zone: 0x%lX - 0x%lX\n", block->start_address,
			   block->start_address + block->size);

		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		for (uint64_t j = 1; j <= cnt_miniblock; ++j) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;
			printf("Miniblock %ld:", j);
			printf("\t\t0x%lX\t\t-\t\t0x%lX\t\t| ", miniblock->start_address,
				   miniblock->start_address + miniblock->size);

			//print permissions ---\n
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
	node_t *block_list = arena->alloc_list->head;
	uint64_t cnt_block = arena->alloc_list->size;

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

void print_from_miniblock(block_t *block, uint64_t address,
						  uint64_t size, uint64_t offset)
{
	uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
	node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

	char *final = calloc(size + 2, sizeof(char));
	if (!final) {
		fprintf(stderr, "calloc failed in print miniblock\n");
		return;
	}

	for (uint64_t j = 0; j < cnt_miniblock ; ++j) {
		miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;
		uint64_t end_mini = miniblock->start_address + miniblock->size - 1;

		if (end_mini < address) {
			miniblock_list = miniblock_list->next;
			offset -= miniblock->size;
			continue;
		}

		if (miniblock->start_address == address) {
			uint8_t perm = miniblock->perm;
			if (perm == 0 || perm == 2 || perm == 1 || perm == 3) {
				error_inv_perm_read();
				free(final);
				return;
			}

			char *content = calloc(miniblock->size + 1, sizeof(char));

			memcpy(content, miniblock->rw_buffer, miniblock->size + 1);
			memcpy(content + miniblock->size, "\0", 1);

			size_t start = 0;
			size_t len = strlen(content);

			while (size && start < len) {
				char c_to_str[2];
				c_to_str[1] = '\0';
				c_to_str[0] = content[start];
				strcat(final, c_to_str);

				start++;
				size--;
			}

			if (start != len || j == cnt_miniblock - 1)
				strcat(final, "\n");

			miniblock_list = miniblock_list->next;

			address = ((miniblock_t *)miniblock_list->data)->start_address;

			free(content);
			continue;
		}

		if (address > miniblock->start_address && address < end_mini) {
			uint8_t perm = miniblock->perm;
			if (perm == 0 || perm == 2 || perm == 1 || perm == 3) {
				error_inv_perm_read();
				free(final);
				return;
			}

			char *content = calloc(miniblock->size + 1, sizeof(char));
			memcpy(content, miniblock->rw_buffer, miniblock->size + 1);

			size_t start = offset;
			size_t len = strlen(content);

			while (size && start < len) {
				char c_to_str[2];
				c_to_str[1] = '\0';
				c_to_str[0] = content[start];
				strcat(final, c_to_str);

				start++;
				size--;
			}

			if (start != len || j == cnt_miniblock - 1)
				strcat(final, "\n");

			offset = 0;

			miniblock_list = miniblock_list->next;
			address = ((miniblock_t *)miniblock_list->data)->start_address;

			free(content);
			continue;
		}
	}

	printf("%s", final);
	free(final);
}

uint8_t copy_to_miniblock(block_t *block, int8_t *data)
{
	uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
	node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

	uint64_t offset = 0;
	for (uint64_t j = 0; j < cnt_miniblock; ++j) {
		miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;

		//check permissions of the miniblock
		uint8_t perm = miniblock->perm;
		if (perm == 0 || perm == 4 || perm == 1 || perm == 5) {
			error_inv_perm_write();
			return 0;
		}

		size_t len = strlen((char *)data + offset);
		size_t cpy_size = (len < miniblock->size) ? len : miniblock->size;

		int8_t *rw_buff = (int8_t *)miniblock->rw_buffer;
		memcpy(rw_buff, (int8_t *)data + offset, cpy_size);
		offset += miniblock->size;

		size_t str_len = strlen(miniblock->rw_buffer);
		if (j == cnt_miniblock - 1)
			memcpy(rw_buff + str_len, "\n", 1);
		else
			memcpy(rw_buff + str_len, "\0", 1);

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
