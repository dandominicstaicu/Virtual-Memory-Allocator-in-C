//Copyright 2023 Dan-Dominic Staicu 311CAb
#pragma once
#include <inttypes.h>
#include <stddef.h>

#include "doubly_linked_list.h"
#include "utils.h"
#include "errors.h"

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

arena_t *alloc_arena(const uint64_t size);
void dealloc_arena(arena_t *arena);

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);
void free_block(arena_t *arena, const uint64_t address, uint64_t final);

void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address,
		   const uint64_t size, int8_t *data);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

block_t *search_alloc(arena_t *arena,
					  const uint64_t start, const uint64_t last);

uint8_t copy_to_miniblock(block_t *block, int8_t *data);

void print_from_miniblock(block_t *block, uint64_t address,
						  uint64_t size, uint64_t offset);

uint8_t num_perm(int8_t *permission);

void print_perm(uint8_t permissions);
