//Copyright 2023 Dan-Dominic Staicu 311CAb
#include "vma.h"

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
