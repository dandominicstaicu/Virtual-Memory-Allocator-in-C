#ifndef _RW_H_
#define _RW_H_

#include "utils.h"
#include "vma.h"

void f_write(arena_t *arena);

void f_read(arena_t *arena);

void read(arena_t *arena, uint64_t address, uint64_t size);

void write(arena_t *arena, const uint64_t address,
		   const uint64_t size, int8_t *data);

uint8_t copy_to_miniblock(block_t *block, int8_t *data);

void print_from_miniblock(block_t *block, uint64_t address,
						  uint64_t size, uint64_t offset);

uint8_t num_perm(int8_t *permission);

void print_perm(uint8_t permissions);

uint8_t perm_print(miniblock_t *miniblock);

void copy_to_final(uint64_t *size, uint64_t *start, char *content, char *final,
				   uint64_t j, uint64_t cnt_miniblock);

void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

void f_mprotect(arena_t *arena);

#endif
