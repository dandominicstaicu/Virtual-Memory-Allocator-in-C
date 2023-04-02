#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>

#include "utils.h"

void error_inv_alloc_block_out(void);

void error_inv_alloc_block_end(void);

void error_inv_alloc_block_alr_aloc(void);

void error_inv_addr_free(void);

void error_inv_addr_read(void);

void error_inv_addr_write(void);

void error_inv_command(void);

void warn_read(uint64_t size);

void warn_write(uint64_t size);

#endif
