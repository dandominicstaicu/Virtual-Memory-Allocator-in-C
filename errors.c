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
	printf("Invalid address for free.\n");
}

void error_inv_addr_read(void)
{
	printf("Invalid address for read.\n");
}

void error_inv_addr_write(void)
{
	printf("Invalid address for write.\n");
}

void error_inv_command(void)
{
	printf("Invalid command. Please try again.\n");
}

void warn_read(uint64_t size)
{
	printf("Warning: size was bigger than the block size. ");
	printf("Reading %ld characters.\n", size);
}

void warn_write(uint64_t size)
{
	printf("Warning: size was bigger than the block size. ");
	printf("Writing %ld characters.\n", size);
}

void error_inv_mprot(void)
{
	printf("Invalid address for mprotect.\n");
}

void error_inv_perm_read(void)
{
	printf("Invalid permissions for read.\n");
}

void error_inv_perm_write(void)
{
	printf("Invalid permissions for read.\n");
}
