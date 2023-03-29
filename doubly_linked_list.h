#ifndef _DOUBLY_LINKED_LIST_H_
#define _DOUBLY_LINKED_LIST_H_

#include "utils.h"

typedef struct node_t node_t;
struct node_t{
   void *data;
   struct node_t *prev, *next;
};

/* TODO : add your implementation for doubly-linked list */
typedef struct {
    node_t *head;
    uint data_size;
    uint size;
} list_t;

list_t *ll_create(unsigned int data_size);
node_t *ll_get_nth_node(list_t* list, unsigned int n);
void ll_add_nth_node(list_t *list, unsigned int n, const void *new_data);
node_t *ll_remove_nth_node(list_t *list, unsigned int n);
void ll_free(list_t **pp_list);

#endif