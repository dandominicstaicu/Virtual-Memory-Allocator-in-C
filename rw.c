#include "rw.h"

void f_read(arena_t *arena)
{
	uint64_t arena_address = 0;
	uint64_t read_size = 0;

	scanf("%lu%lu", &arena_address, &read_size);

	read(arena, arena_address, read_size);
}

void f_write(arena_t *arena)
{
	

	uint64_t arena_address = 0;
	uint64_t write_size = 0;

	scanf("%lu%lu", &arena_address, &write_size);

	int8_t *data = malloc(sizeof(int8_t) * write_size + 1);
	if (!data) {
		fprintf(stderr, "could not alloc data\n");
		exit(-1);
	}
	getchar();
	fgets((char *)data, write_size + 1, stdin);
	//printf("\n\ndata in fwrite: %s\n\n", data);

	write(arena, arena_address, write_size, data);
	//getchar();
}
