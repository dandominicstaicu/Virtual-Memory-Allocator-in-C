#ifndef _ARENA_H_
#define _ARENA_H_

#include <stdio.h>
#include <inttypes.h>

#include "vma.h"

//this file contains the definiton of all functions
//which refer to arena memory manipulation

//read values of the ALLOC_ARENA command and pass them
void f_alloc_arena(arena_t **arena);

//alloc the mem needed for "remembering" the arena
arena_t *alloc_arena(const uint64_t size);

//free all the memory inside the arena
void dealloc_arena(arena_t *arena);

#endif
