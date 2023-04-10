#ifndef _F_BLOCK_H_
#define _F_BLOCK_H_

#include "errors.h"
#include "utils.h"
#include "arena.h"
#include "vma.h"

void f_block(arena_t *arena);

void f_free_block(arena_t *arena);

void both_neighbors(block_t *block, block_t *neighbor_l, block_t *neighbor_r,
					miniblock_t *first_miniblock, arena_t *arena,
					uint64_t size);

void just_right(block_t *block, block_t *neighbor_r, arena_t *arena,
				miniblock_t *first_miniblock, uint64_t size, uint64_t address);

void just_left(block_t *block, block_t *neighbor_l, arena_t *arena,
			   miniblock_t *first_miniblock, uint64_t size);

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);

void free_block(arena_t *arena, const uint64_t address, uint64_t final);

#endif
