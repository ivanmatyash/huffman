/**
 * @file heap.c
 * @author Ivan Matsiash
 * @brief Implementation of deafult operations with heap
 *
 * This file contains implementation of default operation to work with heap, such as create new heap, delete node, insert node, get minimum node, etc.
 * @copyright Copyright (c) 2017, Ivan Matsiash / FAMCS BSU, Epam
 */
#include "heap.h"

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
