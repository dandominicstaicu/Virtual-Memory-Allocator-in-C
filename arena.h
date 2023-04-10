#ifndef _ARENA_H_
#define _ARENA_H_

#include <stdio.h>
#include <inttypes.h>

#include "vma.h"

void f_alloc_arena(arena_t **arena);

arena_t *alloc_arena(const uint64_t size);

void dealloc_arena(arena_t *arena);

#endif
