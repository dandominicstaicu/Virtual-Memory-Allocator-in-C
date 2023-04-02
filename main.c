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
			dealloc_arena(arena);
			break;
		case 2:
			f_block(arena);
			break;
		case 3:
			f_free_block(arena);
			break;
		case 4:
			f_read(arena);
			break;
		case 5:
			f_write(arena);
			break;
		case 6:
			f_pmap(arena);
			break;
		default:
			error_inv_command();
			//DELETE IT OR DIE//
			goto kill_me;
			break;
		}
	}
	
kill_me:
    return 0;
}
