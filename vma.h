//Copyright 2023 Dan-Dominic Staicu 311CAb
#ifndef _VMA_H_
#define _VMA_H_

#include <inttypes.h>
#include <stddef.h>

#include "doubly_linked_list.h"
#include "utils.h"
#include "errors.h"
#include "rw.h"

void pmap(const arena_t *arena);

#endif
