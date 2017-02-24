#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
struct heapNode
{
	int priority;
	unsigned char value;
	struct heapNode *left;
	struct heapNode *right;
} typedef heapNode;

struct heap
{
	int maxSize;
	int curSize;
	struct heapNode *nodes;
} typedef heap;

heap* create_heap(int);
void delete_heap();
void swap_nodes_heap(heapNode*, heapNode*);
heapNode get_min_node_heap(heap*);
bool insert_node_heap(heap*, int, unsigned char, heapNode*, heapNode*);
heapNode remove_min_node_heap(heap*);
