#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>

void error_inv_alloc_block(void);

void error_inv_addr_free(void);

void error_inv_addr_read(void);

void error_inv_addr_write(void);

void error_inv_command(void);

#endif
