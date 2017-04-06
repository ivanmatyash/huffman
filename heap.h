/**
 * @file heap.h
 * @author Ivan Matsiash
 * @brief Header file for heap functions
 
 * This file contains declare of heap and definitions of default functions, that used in working with heap.
 * @copyright Copyright (c) 2017, Ivan Matsiash / FAMCS BSU, Epam
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * Struct of heap node
 */
struct heap_node_struct
{
/**
 * @name Fields of every node in heap
 */
 /*@{*/
	int priority; /**< priority of node == frequency of this symbol in input file*/
	unsigned char value; /**< value of node == symbol in input file*/
	struct heap_node_struct *left; /**< pointer to left son of this node*/
	struct heap_node_struct *right; /**< pointer to right son of this node*/
	unsigned int huffman_code; /**< huffman code for this node*/
	unsigned int amount_of_significant_bits; /**< amount of significant bits for this node*/
} typedef heap_node;

/**
 * Struct of heap
 */
struct heap
{
/**
 * @name Parameters of heap
 */
 /*@{*/
	int max_size; /**< the maximum size of heap*/
	int cur_size; /**< current size of heap*/
	heap_node *nodes; /**< pointer to array of nodes heap*/
 /*@}*/
} typedef heap;

heap* create_heap(int);
void delete_heap(heap*);
void swap_nodes_heap(heap_node*, heap_node*);
heap_node get_min_node_heap(heap*);
bool insert_node_heap(heap*, int, unsigned char, heap_node*, heap_node*);
heap_node remove_min_node_heap(heap*);
