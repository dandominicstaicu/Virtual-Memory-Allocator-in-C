#include "doubly_linked_list.h"

//alloc nmemory for a new empty dll
list_t *ll_create(unsigned int data_size)
{
	//alloc a new list and check
	list_t *list = (list_t *)malloc(sizeof(list_t));
	DIE(!list, "could not alloc list\n");

	//init the empty list
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;

	return list;
}

//return a pointer to the nth node in the list; index starts from 0;
//n >= 0 granted; n >= list->size is possible
node_t *ll_get_nth_node(list_t *list, unsigned int n)
{
	//check if list exits
	if (!list)
		return NULL;

	//map n between 0 and last index
	unsigned int poz = 0;
	poz = n % list->size;

	//iterate to the n-th node
	node_t *node = list->head;

	//if n == 0 then this for won't execute, so it will return list->head
	for (unsigned int i = 0; i < poz; ++i)
		node = node->next;

	return node;
}

//create a new node on the nth position in the list containing the *new_data
//if n >= list->size, add on the last position in the list; n >= 0 granted
void ll_add_nth_node(list_t *list, unsigned int n,
					 const void *new_data)
{
	//check if list exists
	if (!list)
		return;

	//alloc new mem for a new node
	node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
	DIE(!new_node, "could not alloc new_node\n");

	//alloc new mem for new data inside the node
	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "could not alloc data for new node\n");

	//copy the new data in the new node
	memcpy(new_node->data, new_data, list->data_size);

	//check if add first or empty list
	node_t *node;

	//check if add first or empty list
	if (list->size == 0) {
		new_node->next = new_node;
		new_node->prev = new_node;

		list->head = new_node;
		++list->size;

		return;
	} else if (n == 0) {
		node = list->head;

		new_node->next = node;
		new_node->prev = node->prev;

		node->prev->next = new_node;
		node->prev = new_node;

		list->head = new_node;
		++list->size;

		return;
	}

	//map n below list size
	if (n >= list->size)
		n = list->size;

	//getthe previous (to the new_node) node
	node = ll_get_nth_node(list, n - 1);

	//create the new links
	new_node->next = node->next;
	new_node->prev = node;

	node->next->prev = new_node;
	node->next = new_node;

	++list->size;
}

//remove the nth node from the *list; positions indexed starting from 0
//function returns a pointer to the removed node; free is not included
//n >= 0 is granted
node_t *ll_remove_nth_node(list_t *list, unsigned int n)
{
	if (!list || !list->size)
		return NULL;

	if (n >= list->size)
		n = list->size - 1;

	node_t *rmv;

	if (n == 0) {
		rmv = list->head;
		list->head = rmv->next;
	} else {
		rmv = ll_get_nth_node(list, n);
	}

	rmv->next->prev = rmv->prev;
	rmv->prev->next = rmv->next;

	if (--list->size == 0)
		list->head = NULL;

	return rmv;
}

//free the memory of all the nodes in the list; in the end free the list
void ll_free(list_t **pp_list)
{
	//check if list exists
	if (!pp_list)
		return;

	//iterate through all the nodes
	node_t *node;
	while ((*pp_list)->size) {
		//removing the first node in the list every time
		//because this is the fastest removal
		node = ll_remove_nth_node(*pp_list, 0);

		//free memory of the data field and the node itself
		free(node->data);
		free(node);
	}

	//free the memory of the list and make it point to NULL
	free(*pp_list);
	*pp_list = NULL;
}

unsigned int dll_get_size(list_t *list)
{
	return list->size;
}
