#ifndef _F_BLOCK_H_
#define _F_BLOCK_H_

#include "errors.h"
#include "utils.h"
#include "arena.h"
#include "vma.h"

void f_block(arena_t *arena);

void f_free_block(arena_t *arena);

#endif