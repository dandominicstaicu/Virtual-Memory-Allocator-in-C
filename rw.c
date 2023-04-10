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

	while (buff_size < write_size) {
		fgets((char *)buffer, write_size - buff_size + 1, stdin);

		size_t line_len = strlen((char *)buffer);
		memcpy(data + buff_size, buffer, line_len);

		buff_size += line_len;
		memset(buffer, 0, write_size + 1);
	}

	write(arena, arena_address, write_size, data);

	free(buffer);
	free(data);
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	int8_t found = 0;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t start_block = block->start_address;
		uint64_t end_block = start_block + block->size - 1;

		if (address >= start_block && address <= end_block) {
			uint64_t offset = 0;
			if (address > start_block)
				offset = address - start_block;

			if (address + size - 1 > end_block) {
				uint64_t available_space = end_block - address + 1;
				warn_read(available_space);
			}

			print_from_miniblock(block, address, size, offset);

			found = 1;
		}
		block_list = block_list->next;
	}

	if (!found)
		error_inv_addr_read();
}

void write(arena_t *arena, const uint64_t address,
		   const uint64_t size, int8_t *data)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	uint8_t found = 0;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t start_block = block->start_address;
		uint64_t end_block = block->start_address + block->size - 1;

		if (address >= start_block && address <= end_block) {
			found = 1;

			if (address + size - 1 <= end_block) {
				//copy data in miniblocks
				copy_to_miniblock(block, data);
			} else {
				uint64_t available_space = end_block - address + 1;
				int8_t *new_data = calloc(available_space + 2, sizeof(int8_t));
				if (!new_data) {
					fprintf(stderr, "could not alloc new data\n");
					exit(-1);
				}

				memcpy(new_data, data, available_space);
				memcpy(new_data + available_space + 1, "\0", 1);

				//copy new_data in miniblocks
				uint8_t succes = copy_to_miniblock(block, new_data);

				if (succes)
					warn_write(available_space);

				free(new_data);
			}
		}
		block_list = block_list->next;
	}

	if (!found)
		error_inv_addr_write();
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	uint64_t cnt_block = arena->alloc_list->size;
	node_t *block_list = arena->alloc_list->head;

	for (uint64_t i = 0; i < cnt_block; ++i) {
		block_t *block = (block_t *)block_list->data;

		uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
		node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

		for (uint64_t j = 0; j < cnt_miniblock; ++j) {
			miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;
			uint64_t start_mini = miniblock->start_address;

			if (start_mini == address) {
				miniblock->perm = num_perm(permission);
				return;
			}

			miniblock_list = miniblock_list->next;
		}

		block_list = block_list->next;
	}

	error_inv_mprot();
}

uint8_t num_perm(int8_t *permission)
{
	uint8_t perm = 0;

	if (strstr((char *)permission, "PROT_NONE"))
		return 0;

	if (strstr((char *)permission, "PROT_READ"))
		perm += 4;

	if (strstr((char *)permission, "PROT_WRITE"))
		perm += 2;

	if (strstr((char *)permission, "PROT_EXEC"))
		perm += 1;

	return perm;
}

void print_from_miniblock(block_t *block, uint64_t address,
						  uint64_t size, uint64_t offset)
{
	uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
	node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

	char *final = calloc(size + 2, sizeof(char));
	if (!final) {
		fprintf(stderr, "calloc failed in print miniblock\n");
		return;
	}

	for (uint64_t j = 0; j < cnt_miniblock ; ++j) {
		miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;
		uint64_t end_mini = miniblock->start_address + miniblock->size - 1;

		if (end_mini < address) {
			miniblock_list = miniblock_list->next;
			offset -= miniblock->size;
			continue;
		}

		if (miniblock->start_address == address) {
			if (!perm_print(miniblock)) {
				free(final);
				return;
			}

			char *content = calloc(miniblock->size + 1, sizeof(char));

			memcpy(content, miniblock->rw_buffer, miniblock->size + 1);
			memcpy(content + miniblock->size, "\0", 1);

			size_t start = 0;
			copy_to_final(&size, &start, content, final, j, cnt_miniblock);

			miniblock_list = miniblock_list->next;

			address = ((miniblock_t *)miniblock_list->data)->start_address;

			free(content);
			continue;
		}

		if (address > miniblock->start_address && address < end_mini) {
			if (!perm_print(miniblock)) {
				free(final);
				return;
			}

			char *content = calloc(miniblock->size + 1, sizeof(char));
			memcpy(content, miniblock->rw_buffer, miniblock->size + 1);

			size_t start = offset;
			copy_to_final(&size, &start, content, final, j, cnt_miniblock);
			offset = 0;

			miniblock_list = miniblock_list->next;
			address = ((miniblock_t *)miniblock_list->data)->start_address;

			free(content);
			continue;
		}
	}

	printf("%s", final);
	free(final);
}

void copy_to_final(uint64_t *size, uint64_t *start, char *content, char *final,
				   uint64_t j, uint64_t cnt_miniblock)
{
	size_t len = strlen(content);

	while (*size && *start < len) {
		char c_to_str[2];
		c_to_str[1] = '\0';
		c_to_str[0] = content[*start];
		strcat(final, c_to_str);

		*start = *start + 1;
		*size = *size - 1;
	}

	if (*start != len || j == cnt_miniblock - 1)
		strcat(final, "\n");
}

uint8_t perm_print(miniblock_t *miniblock)
{
	uint8_t perm = miniblock->perm;
	if (perm == 0 || perm == 2 || perm == 1 || perm == 3) {
		error_inv_perm_read();
		return 0;
	}

	return 1;
}

uint8_t copy_to_miniblock(block_t *block, int8_t *data)
{
	uint64_t cnt_miniblock = ((list_t *)block->miniblock_list)->size;
	node_t *miniblock_list = ((list_t *)block->miniblock_list)->head;

	uint64_t offset = 0;
	for (uint64_t j = 0; j < cnt_miniblock; ++j) {
		miniblock_t *miniblock = (miniblock_t *)miniblock_list->data;

		//check permissions of the miniblock
		uint8_t perm = miniblock->perm;
		if (perm == 0 || perm == 4 || perm == 1 || perm == 5) {
			error_inv_perm_write();
			return 0;
		}

		size_t len = strlen((char *)data + offset);
		size_t cpy_size = (len < miniblock->size) ? len : miniblock->size;

		int8_t *rw_buff = (int8_t *)miniblock->rw_buffer;
		memcpy(rw_buff, (int8_t *)data + offset, cpy_size);
		offset += miniblock->size;

		size_t str_len = strlen(miniblock->rw_buffer);
		if (j == cnt_miniblock - 1)
			memcpy(rw_buff + str_len, "\n", 1);
		else
			memcpy(rw_buff + str_len, "\0", 1);

		miniblock_list = miniblock_list->next;
	}
	return 1;
}

void print_perm(uint8_t permissions)
{
	switch (permissions) {
	case 0:
		printf("---\n");
		break;
	case 1:
		printf("--X\n");
		break;
	case 2:
		printf("-W-\n");
		break;
	case 3:
		printf("-WX\n");
		break;
	case 4:
		printf("R--\n");
		break;
	case 5:
		printf("R-X\n");
		break;
	case 6:
		printf("RW-\n");
		break;
	case 7:
		printf("RWX\n");
		break;
	default:
		break;
	}
}

void f_mprotect(arena_t *arena)
{
	uint64_t arena_address = 0;
	int8_t permissions[50] = "";

	scanf("%lu", &arena_address);
	fgets((char *)permissions, 50, stdin);

	mprotect(arena, arena_address, permissions);
}
