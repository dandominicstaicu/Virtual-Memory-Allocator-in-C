//Copyright 2023 Dan-Dominic Staicu 311CAb
#ifndef _RW_H_
#define _RW_H_

#include "utils.h"
#include "vma.h"

// read the parameters for command WRITE and pass them further
void f_write(arena_t *arena);

// read the parameters for command READ and pass them further
void f_read(arena_t *arena);

// search the address in mem and get ready to copy it in the arena
void read(arena_t *arena, uint64_t address, uint64_t size);

// search the address in mem and get the data ready to be copied in a block
void write(arena_t *arena, const uint64_t address,
		   const uint64_t size, int8_t *data);

//validate perms and write the new data in the list
uint8_t copy_to_miniblock(block_t *block, int8_t *data);

//validate perms read from the miniblock's mem and print it
void print_from_miniblock(block_t *block, uint64_t address,
						  uint64_t size, uint64_t offset);

// transform parameters of MPROTECT command to digits according to the standard
uint8_t num_perm(int8_t *permission);

//check if the miniblock has permissions to read
uint8_t perm_print(miniblock_t *miniblock);

//copy from the content string (of a single mini) to the final string
void copy_to_final(uint64_t *size, uint64_t *start, char *content, char *final,
				   uint64_t j, uint64_t cnt_miniblock);

// search for the given address and set the permission field as interpreted
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);

//read parameters of MPROTECT command and pass them
void f_mprotect(arena_t *arena);

#endif
