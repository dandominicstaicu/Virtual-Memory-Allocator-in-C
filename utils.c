#include "utils.h"

short hash_command(char *command)
{
	if (strcmp(command, "ALLOC_ARENA") == 0)
		return 0;
	else if (strcmp(command, "DEALLOC_ARENA") == 0)
		return 1;
	else if (strcmp(command, "ALLOC_BLOCK") == 0)
		return 2;
	else if (strcmp(command, "FREE_BLOCK") == 0)
		return 3;
	else if (strcmp(command, "READ") == 0)
		return 4;
	else if (strcmp(command, "WRITE") == 0)
		return 5;
	else if (strcmp(command, "PMAP") == 0)
		return 6;
	else if (strcmp(command, "MPROTECT") == 0)
		return 7;

	return -1;
}
