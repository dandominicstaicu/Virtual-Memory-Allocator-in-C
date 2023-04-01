#include "errors.h"

void error_inv_alloc_block_out(void)
{
	printf("The allocated address is outside the size of arena\n");
}

void error_inv_alloc_block_end(void)
{
	printf("The end address is past the size of the arena\n");
}

void error_inv_alloc_block_alr_aloc(void)
{
	printf("This zone was already allocated.\n");
}

void error_inv_addr_free(void)
{

}

void error_inv_addr_read(void)
{

}

void error_inv_addr_write(void)
{

}

void error_inv_command(void)
{
	
}