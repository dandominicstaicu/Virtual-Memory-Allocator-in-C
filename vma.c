//Copyright 2023 Dan-Dominic Staicu 311CAb
#include "vma.h"

arena_t *alloc_arena(const uint64_t size)
{
    arena_t *arena = malloc(sizeof(arena_t));
    
    if (!arena) {
        fprintf(stderr, "could not alloc mem in alloc_arena\n");
        return NULL;
    }

    arena->arena_size = size;
    arena->alloc_list = NULL;

    return arena;
}

void dealloc_arena(arena_t *arena)
{
    // ll_free(arena->alloc_list);

    // //free(arena->alloc_list);
    // free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
    
}

void free_block(arena_t *arena, const uint64_t address)
{

}

void read(arena_t *arena, uint64_t address, uint64_t size)
{

}

void write(arena_t *arena, const uint64_t address, const uint64_t size, int8_t *data)
{

}

void pmap(const arena_t *arena)
{

}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{

}
