#include "vma.h"
#include <stdio.h>
#include "utils.h"
#include "arena.h"
#include "f_block.h"
#include "rw.h"

int main(void)
{
	
	char command[20];
	arena_t *arena = NULL;

	while (scanf("%s", command)) {
		switch (hash_command(command)) {
		case 0:
			f_alloc_arena(&arena);
			break;
		case 1:
			f_dealloc_arena();
			break;
		case 2:
			f_block(arena);
			/*
			node_t *block_list = (node_t *)arena->alloc_list->head;
			uint64_t cnt_block = arena->alloc_list->size;

			for (uint64_t i = 1; i <= cnt_block; ++i) {
				block_t *block = (block_t *)block_list->data;

				//TODO debug
				printf("size of block %ld: %ld\n", i, block->size);
				//printf("this shit: %d", ((list_t *)block->miniblock_list)->size);
				//all_miniblocks += ((list_t *)block->miniblock_list)->size;

				block_list = block_list->next;
			}
			*/
			break;
		case 3:
			f_free_block();
			break;
		case 4:
			f_read();
			break;
		case 5:
			f_write();
			break;
		case 6:
			f_pmap(arena);
			break;
		default:
			goto kill_me;
			printf("comanda gresita\n");
			break;
		}
	}
	
kill_me:
    return 0;
}
