#include "vma.h"
#include "utils.h"
#include "arena.h"
#include "f_block.h"
#include "rw.h"

int main(void)
{
	//keep command string as a static array because strlen(command) < 20
	char command[20];
	arena_t *arena = NULL;

	while (scanf("%s", command)) {
		//use a "hash" that returns a number for each command
		//in order to use switch
		switch (hash_command(command)) {
		case 0:
			f_alloc_arena(&arena);
			break;
		case 1:
			dealloc_arena(arena);
			//stop the program after deallocing everything
			return 0;
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
			pmap(arena);
			break;
		case 7:
			f_mprotect(arena);
			break;
		default:
		//in case command is not recognized
			error_inv_command();
			break;
		}
	}

	return 0;
}

