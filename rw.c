#include "rw.h"

void f_read(void)
{
	uint64_t arena_address = 0;
	uint64_t write_size = 0;

	scanf("%lu%lu", &arena_address, &write_size);
}

void f_write(void)
{
	uint64_t arena_address = 0;
	uint64_t write_size = 0;

	scanf("%lu%lu", &arena_address, &write_size);
}

void f_pmap(const arena_t *arena)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;
	uint64_t free_size = arena->arena_size;
	uint64_t all_miniblocks = 0;

	for (uint64_t i = 1; i <= cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;
		free_size -= block->size;
		//TODO debug
		printf("this shit: %d", ((list_t *)block->miniblock_list)->size);
		all_miniblocks += ((list_t *)block->miniblock_list)->size;

		block_list = block_list->next;
	}

	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	printf("Free memory: 0x%lX bytes\n", free_size);
	printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
	printf("Number of allocated miniblocks: %ld\n", all_miniblocks);
	printf("\n");

	for (uint64_t i = 1; i <= cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;
		printf("Block %ld begin\n", i);
		printf("Zone: 0x%lX - 0x%lX\n", block->start_address, block->start_address + block->size);
	//TODO not sure about zone line idk +/- 1 idk idk idk man i go crazy

		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		for (uint64_t i = 1; i <= cnt_miniblock; ++i) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;
			printf("\t\t0x%lX\t\t-\t\t0x%lX\t\t| RW-\n", miniblock->start_address, miniblock->start_address + miniblock->size);
			//TODO not sure about zone line idk +/- 1 idk idk idk man i go crazy
		}

		printf("Block %ld end\n", i);
		block_list = block_list->next;
	}
}
