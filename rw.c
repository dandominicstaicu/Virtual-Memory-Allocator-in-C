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

	int8_t *data = calloc(write_size + 1, sizeof(int8_t));
	if (!data) {
		fprintf(stderr, "could not alloc data\n");
		exit(-1);
	}
	getchar();

	int8_t *buffer = calloc(write_size + 1, sizeof(int8_t));
	size_t buff_size = 0;

	/*SKULL SKULL SKULL SKULL SKULL SKULL SKULL*/
	while (buff_size < write_size) {
		fgets((char *)buffer, write_size - buff_size + 1, stdin);
		
		size_t line_len = strlen((char *)buffer);
		memcpy(data + buff_size, buffer, line_len);

		buff_size += line_len;
		memset(buffer, 0, write_size + 1);
	}
	/*SKULL SKULL SKULL SKULL SKULL SKULL SKULL*/
	
	write(arena, arena_address, write_size, data);

	free(buffer);
	free(data);
}
