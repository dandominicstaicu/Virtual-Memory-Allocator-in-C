#include "utils.h"

short hash_command(char *command)
{
	if (strcmp(command, "ALLOC_ARENA"))
		return 0;
	else if (strcmp(command, "DEALLOC_ARENA"))
		return 1;
	else if (strcmp(command, "ALLOC_BLOCK"))
		return 2;
	else if (strcmp(command, "FREE_BLOCK"))
		return 3;
	else if (strcmp(command, "READ"))
		return 4;
	else if (strcmp(command, "WRITE"))
		return 5;
	else if (strcmp(command, "PMAP"))
		return 6;
}