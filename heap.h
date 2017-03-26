#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct heap_node_struct
{
	int priority;
	unsigned char value;
	struct heap_node_struct *left;
	struct heap_node_struct *right;
	unsigned int huffman_code;
	unsigned int amount_of_significant_bits;
} typedef heap_node;

struct heap
{
	int max_size;
	int cur_size;
	heap_node *nodes;
} typedef heap;

heap* create_heap(int);
void delete_heap();
void swap_nodes_heap(heap_node*, heap_node*);
heap_node get_min_node_heap(heap*);
bool insert_node_heap(heap*, int, unsigned char, heap_node*, heap_node*);
heap_node remove_min_node_heap(heap*);
