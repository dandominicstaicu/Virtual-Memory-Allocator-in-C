#include "vma.h"
#include <stdio.h>
#include "utils.h"
#include "arena.h"

int main(void)
{
	char command[20];

	while (scanf("%s", command)) {
		switch (hash_command(command)) {
		case 0:
			f_arena();
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		default:
			break;
		}
	}

    return 0;
}