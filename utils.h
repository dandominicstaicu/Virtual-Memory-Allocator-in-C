#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

typedef unsigned int uint;

#define DIE(assertion, call_description)		\
	do {										\
		if (assertion) {						\
			fprintf(stderr, "(%s, %d): ",		\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}										\
	} while (0)

short hash_command(char *command);

#endif