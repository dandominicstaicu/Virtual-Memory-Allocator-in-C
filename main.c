#include "vma.h"
#include <stdio.h>
#include "utils.h"
#include "arena.h"
#include "f_block.h"
#include "rw.h"

int main(void)
{
	char command[20];

	while (scanf("%s", command)) {
		switch (hash_command(command)) {
		case 0:
			f_alloc_arena();
			break;
		case 1:
			f_dealloc_arena();
			break;
		case 2:
			f_block();
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
			f_pmap();
			break;
		default:
			break;
		}
	}

    return 0;
}