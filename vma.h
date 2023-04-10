//Copyright 2023 Dan-Dominic Staicu 311CAb
#ifndef _VMA_H_
#define _VMA_H_

#include "doubly_linked_list.h"
#include "utils.h"
#include "errors.h"

//print the content of the arena
void pmap(const arena_t *arena);

//used by PMAP to interpret digits as RWX format
void print_perm(uint8_t permissions);

#endif
