/**
 * @file heap.c
 * @author Ivan Matsiash
 * @brief Implementation of deafult operations with heap
 *
 * This file contains implementation of default operation to work with heap, such as create new heap, delete node, insert node, get minimum node, etc.
 * @copyright Copyright (c) 2017, Ivan Matsiash / FAMCS BSU, Epam
 */
#include "heap.h"

/**
 * @brief Function for creating new heap

 * Function create new heap of specified size.
 * @param size the maximum amount of elements in new heap
 * @return pointer of new heap
 */
heap* create_heap(int size)
{
	struct heap *h;
	h = malloc(sizeof(*h));
	if (h != NULL) {
		h->max_size = size;
		h->cur_size = 0;
		h->nodes = malloc((size + 1) * sizeof(heap_node));

		if (h->nodes == NULL) {
			free(h);
			return NULL;
		}
	}

	return h;	
}

/**
 * @brief Function for deleting heap

 * Function delete heap and freeing allocated memory.
 * @param h pointer of deleting heap
 */
void delete_heap(heap* h)
{
	free(h->nodes);
	free(h);
}

void swap_nodes_heap(heap_node *a, heap_node *b)
{
	heap_node temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * @brief Function for getting minimum element from heap

 * Function return the minimum node from heap. Minimum - by key (priority).
 * @param h pointer to heap
 * @return minimum node or invalid node in case of error
 */
heap_node get_min_node_heap(heap* h)
{
	heap_node invalid_node = {-1, -1};
	
	if (h->cur_size > 0) {
		return h->nodes[1];
	} else {
		fprintf(stderr, "heap is empty\n");
		return invalid_node;
	}
}

/**
 * @brief Function for inserting node in heap

 * Function insert new node in exiting heap, and automatically sosrted this heap.
 * @param h pointer to heap
 * @param priority priority of added node (key)
 * @param value value of added node
 * @param left_son pointer to left son of node
 * @param right_son pointer to right son of node
 * @return true if inserting was successfull or false otherwise
 */
bool insert_node_heap(heap* h, int priority, unsigned char value, heap_node* left_son, heap_node* right_son)
{
	if (h->cur_size >= h->max_size)	{
		fprintf(stderr, "Invalid insert, heap is full!\n");
		return false;
	}

	h->cur_size++;
	h->nodes[h->cur_size].priority = priority;
	h->nodes[h->cur_size].value = value;
	h->nodes[h->cur_size].left = left_son;
	h->nodes[h->cur_size].right = right_son;
	h->nodes[h->cur_size].huffman_code = 0;
	h->nodes[h->cur_size].amount_of_significant_bits = 0;	
	
	for (int i = h->cur_size; i > 1 && h->nodes[i].priority < h->nodes[i / 2].priority; i = i / 2) {
		swap_nodes_heap(&h->nodes[i], &h->nodes[i/2]);
	}

	return true;
}

/**
 * @brief Function for removing minimum element from heap

 * Function return minimum node and remove it from heap.
 * @param h pointer to heap
 * @return pointer to minimum heap or pointer to invalid node in case of an error
 */
heap_node remove_min_node_heap(heap *h)
{
	if (h->cur_size == 0) {
		heap_node invalid_node = {-1, -1};
		fprintf(stderr, "Invalid removing min. Heap is empty!\n");
		return invalid_node;
	}

	swap_nodes_heap(&h->nodes[1], &h->nodes[h->cur_size]);

	int k = 0, n = 0, j = 0;

	for (k = 1, n = h->cur_size - 1; 2 * k <= n; k = j) {
		j = 2 * k;
		if (j < n && h->nodes[j].priority > h->nodes[j + 1].priority) {
			j++;
		}
	
		if (h->nodes[k].priority <= h->nodes[j].priority) {
			break;
		}

		swap_nodes_heap(&h->nodes[k], &h->nodes[j]);
	}

	return h->nodes[h->cur_size--];
}
