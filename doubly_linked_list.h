//Copyright 2023 Dan-Dominic Staicu 311CAb
#ifndef _DOUBLY_LINKED_LIST_H_
#define _DOUBLY_LINKED_LIST_H_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct node_t node_t;

struct node_t {
	void *data;
	struct node_t *prev, *next;
};

typedef struct {
	node_t *head, *tail;
	unsigned int data_size;
	unsigned int size;
} list_t;

#define DIE(assertion, call_description)		\
	do {										\
		if (assertion) {						\
			fprintf(stderr, "(%s, %d): ",		\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}										\
	} while (0)

list_t *ll_create(unsigned int data_size);

node_t *ll_get_nth_node(list_t *list, unsigned int n);

void ll_add_nth_node(list_t *list, unsigned int n, const void *new_data);

node_t *ll_remove_nth_node(list_t *list, unsigned int n);

void ll_free(list_t **pp_list);

unsigned int ll_get_size(list_t *list);

#endif
