//Copyright 2023 Dan-Dominic Staicu 311CAb
#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

#include "doubly_linked_list.h"

#define DIE(assertion, call_description)		\
	do {										\
		if (assertion) {						\
			fprintf(stderr, "(%s, %d): ",		\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}										\
	} while (0)

typedef struct {
	uint64_t start_address;
	size_t size;
	void *miniblock_list;
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;
	void *rw_buffer;
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	list_t *alloc_list;
} arena_t;

short hash_command(char *command);

/* find check if an area has any part of it unavailable*/
/*return NULL if no blocks, return it if found*/
block_t *search_alloc(arena_t *arena,
					  const uint64_t start, const uint64_t last);

#endif
