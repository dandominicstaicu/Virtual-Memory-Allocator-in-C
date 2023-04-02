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
			break;
		case 3:
			f_free_block(arena);
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
