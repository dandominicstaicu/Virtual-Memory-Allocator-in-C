#ifndef _F_BLOCK_H_
#define _F_BLOCK_H_

#include "errors.h"
#include "utils.h"
#include "arena.h"
#include "vma.h"

/* this file includes all the definitions of functions which manipulate
the memory of the blocks and miniblocks used in the arena */

// read the parameters of ALLOC_BLOCK command and check for errors
void f_block(arena_t *arena);

// read the parameters of FREE_BLOCK command and check for errors
void f_free_block(arena_t *arena);

/* alloc memory for a new miniblock at the given address and
order/reorder/resize the blocks in the arena list */
void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);

/* free the memory of a given miniblock and decide how to resize the block
where the miniblock was found, if there is the case*/
void free_block(arena_t *arena, const uint64_t address, uint64_t final);

/* in case the new miniblock has both the left and right neighbors, should
destroy and free the older blocks and create a new one whit all the minis*/
void both_neighbors(block_t *block, block_t *neighbor_l, block_t *neighbor_r,
					miniblock_t *first_miniblock, arena_t *arena,
					uint64_t size);
/* in case there is only one neighbor in the right of the new one, sould resize
the older block and add the new miniblock to it */
void just_right(block_t *block, block_t *neighbor_r, arena_t *arena,
				miniblock_t *first_miniblock, uint64_t size, uint64_t address);

/* in case there is only one neighbor in the left of the new one, sould resize
the older block and add the new miniblock to it */
void just_left(block_t *block, block_t *neighbor_l, arena_t *arena,
			   miniblock_t *first_miniblock, uint64_t size);

#endif
