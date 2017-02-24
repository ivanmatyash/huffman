#include "heap.h"
#include <stdio.h>

heap* create_heap(int size)
{
	struct heap *h;
	h = malloc(sizeof(*h));
	if (h != NULL)
	{
		h->maxSize = size;
		h->curSize = 0;
		h->nodes = malloc((size + 1) * sizeof(heapNode));
		

		if (h->nodes == NULL)
		{
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

void swap_nodes_heap(heapNode *a, heapNode *b)
{
	heapNode temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

heapNode get_min_node_heap(heap* h)
{
	heapNode invalidNode = {-1, -1};
	
	if (h->curSize > 0)
	{
		return h->nodes[1];
	}
	else
	{
		fprintf(stderr, "heap is empty\n");
		return invalidNode;
	}
}

bool insert_node_heap(heap* h, int priority, unsigned char value, heapNode* leftSon, heapNode* rightSon)
{
	if (h->curSize >= h->maxSize)
	{
		fprintf(stderr, "Invalid insert, heap is full!\n");
		return false;
	}

	h->curSize++;
	h->nodes[h->curSize].priority = priority;
	h->nodes[h->curSize].value = value;
	h->nodes[h->curSize].left = leftSon;
	h->nodes[h->curSize].right = rightSon;

	int i = 0;
	for (i = h->curSize; i > 1 && h->nodes[i].priority < h->nodes[i / 2].priority; i = i / 2)
	{
		swap_nodes_heap(&h->nodes[i], &h->nodes[i/2]);
	}

	return true;
}

heapNode remove_min_node_heap(heap *h)
{
	if (h->curSize == 0)
	{
		heapNode invalidNode = {-1, -1};
		fprintf(stderr, "Invalid removing min. Heap is empty!\n");
		return invalidNode;
	}
	swap_nodes_heap(&h->nodes[1], &h->nodes[h->curSize]);

	int k, n, j;

	for (k = 1, n = h->curSize - 1; 2 * k <= n; k = j)
	{
		j = 2 * k;
		if (j < n && h->nodes[j].priority > h->nodes[j + 1].priority)
		{
			j++;
		}
	
	
		if (h->nodes[k].priority <= h->nodes[j].priority)
		{
			break;
		}

		swap_nodes_heap(&h->nodes[k], &h->nodes[j]);
	}
	return h->nodes[h->curSize--];
}
