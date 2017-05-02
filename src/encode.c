/**
 * @file encode.c
 * @author Ivan Matsiash
 * @brief Implementation of encoding functions
 *
 * This file contains implementations of functions, that used in processes of encoding input file.
 * @copyright Copyright (c) 2017, Ivan Matsiash / FAMCS BSU, Epam
 */
#include "encode.h"
#include "heap.h"
#include <string.h>
#include <time.h>

#define SIZE_BUF 20000
#define SIZE_TABLE 256
#define SIZE_BUF_FOR_WRITE 20000

/**
 * @brief Function print array to stdout

 * Function print every element of array to stdout
 * @param array array of int for printing
 * @param ar array of long for printing
 * @param size amount of elements in arrays
 */
void showArray(unsigned int* array, unsigned long *ar,  int size)
{
	printf("\n\nSHOW ARRAY CHAR\n\n");
	for (int i = 0; i < size; i++) {
		if (array[i] != 0)
		printf("%d: %lu - %d\n", i, ar[i], array[i]);
	}
}

/**
 * @brief Function update frequency of symbols

 * Function update frequency of symbols by blocks
 * @param array_freq array of frequency of symbols
 * @param buffer block of input file
 */
void update_freq(int* array_freq, unsigned char *buffer)
{
	for (int i = 0; i < SIZE_BUF; i++) {
		array_freq[buffer[i]]++;
	}
}

/**
 * @brief Function for getintg frequency of symbols

 * Function read elements from input file by buffers, and call function update_frequency that update frequency of symbols
 * @param file_name the name of input file
 * @param array_freq array of frequency of symbols
 * @return -1 in a case of error, 0 in a case of success
 */
int get_frequency(char* file_name, int *array_freq)
{
	FILE* file = fopen(file_name, "rb");
	if (file == NULL) {
		return -1;
	}
	unsigned char buffer[SIZE_BUF] = {0};
	int end_of_file = 1;

	while (end_of_file) {
		memset(buffer, 0, sizeof(unsigned char) * SIZE_BUF);				
		if (fread(buffer, sizeof(unsigned char), SIZE_BUF, file) < SIZE_BUF) {		// read next block from input file
			end_of_file = 0;
		}
		update_freq(array_freq, buffer);
	}
	fclose(file);
	return 0;
}

/**
 * @brief Create huffman tree from heap of nodes

 * Function consistently removing minimum elements from heap, unite its and again insert into heap, while size of heap > 1
 * @param h pointer to heap
 * @param array_heap_nodes pointer to array of all nodes
 */
void create_huffman_tree(heap* h, heap_node *array_heap_nodes)
{
	int counter = 0;
	while(h->cur_size > 1) {
		array_heap_nodes[counter] = remove_min_node_heap(h);
		counter++;
		array_heap_nodes[counter] = remove_min_node_heap(h);							// get 2 minimum-priority nodes from binary heap
	
		int new_priority = array_heap_nodes[counter-1].priority + array_heap_nodes[counter].priority;		
		unsigned char new_value = array_heap_nodes[counter-1].value + array_heap_nodes[counter].value;		
	
		insert_node_heap(h, new_priority, new_value, &array_heap_nodes[counter-1], &array_heap_nodes[counter]); 
		counter++;
	}	
}

/**
 * @brief Function start to creating huffman tree

 * Function inserting symbol-nodes in heap
 * @param h pointer to heap
 * @param array_heap_nodes pointer to array of symbol-nodes
 * @param array_freq pointer to array of frequency of symbols
 */
void get_huffman_tree(heap* h, heap_node *array_heap_nodes, int *array_freq)
{
	for (int i = 0; i < SIZE_TABLE; i++) {
		if (array_freq[i] != 0) {
			insert_node_heap(h, array_freq[i], i, NULL, NULL);			
		}
	}
	create_huffman_tree(h, array_heap_nodes);
}

/**
 * @brief Recursive function painting nodes of huffman tree

 * Function goes from root-node to lists and painting huffman codes for all nodes
 * @param root pointer to root-node of huffman tree
 * @param huffman_codes_array pointer to array of huffman_codes
 * @param amount_of_significant_bits pointer to array of amount of significant bits
 */
void painting_huffman_tree_recursive(heap_node *root, unsigned long *huffman_codes_array, unsigned int *amount_of_significant_bits)
{
	if (root->right != NULL) {
		(root->right)->huffman_code = root->huffman_code << 1;
		(root->right)->huffman_code = (root->right)->huffman_code | 1;
		(root->right)->amount_of_significant_bits = root->amount_of_significant_bits + 1;
		painting_huffman_tree_recursive(root->right, huffman_codes_array, amount_of_significant_bits);	// go right -> concate 1 bit
	}

	if (root->left != NULL)	{
		(root->left)->huffman_code = root->huffman_code << 1;
		(root->left)->amount_of_significant_bits = root->amount_of_significant_bits + 1;
		painting_huffman_tree_recursive(root->left, huffman_codes_array, amount_of_significant_bits);	// go left -> concate 0 bit
	}

	if (root->left == NULL && root->right == NULL) {
		huffman_codes_array[root->value] = root->huffman_code;
		amount_of_significant_bits[root->value] = root->amount_of_significant_bits;			// if node is list - then write huffman code for this node
	}
}

/**
 * @brief Function painting nodes of huffman tree

 * Function goes from root-node to lists and painting huffman codes for all nodes
 * @param h pointer to heap
 * @param huffman_codes_array pointer to array of huffman_codes
 * @param amount_of_significant_bits pointer to array of amount of significant bits
 */
void painting_huffman_tree(heap* h, unsigned long *huffman_codes_array, unsigned int *amount_of_significant_bits)
{
	heap_node root = remove_min_node_heap(h);		// get the root of huffman tree

	if (root.left == NULL && root.right == NULL) {		// case where only one symbol in input file
		huffman_codes_array[root.value] = 0;
		amount_of_significant_bits[root.value] = 1;
	} else {
		painting_huffman_tree_recursive(&root, huffman_codes_array, amount_of_significant_bits);	// if in input file more than 1 symbol, recursive get huffman codes
	}
}

/**
 * @brief Function encoding input file and write codes of huffman in output file

 * Function read block form input file to buffer, encoding this and write huffman codes in output file
 * @param input_file_name the name of input file
 * @param output_file_name the name of output file
 * @param huffman_codes_array pointer to array of huffman codes
 * @param amount_of_significant_bits pointer to array of amount of significant bits
 */
void write_code_in_file(char* input_file_name, char* output_file_name, unsigned long *huffman_codes_array, unsigned int *amount_of_significant_bits)
{
	FILE *input_file = fopen(input_file_name, "rb");
	FILE *output_file = fopen(output_file_name, "wb");

	unsigned long buffer_for_write[SIZE_BUF_FOR_WRITE] = {0};	
	unsigned char buffer[SIZE_BUF] = {0};
	
	long offset = sizeof(int)  + sizeof(int) + sizeof(unsigned long) * SIZE_TABLE + sizeof(unsigned int) * SIZE_TABLE;
	fseek(output_file, offset, SEEK_SET); 				// place for amount of long variables + amount of last bits +  codes + amount of bits

	int number = 0, counter_bits = 0, amount_of_written = 1;
	int end_of_file = 0;
	
	char test_of_empty_file = 0;				// buffer - for reading 1 symbol from input file
	int test_empty = fread(buffer, sizeof(char), 1, input_file);	
	if (test_empty == 0) {						// if read 0 byte
		end_of_file = 1;
		amount_of_written = 0;				
		fprintf(stderr, "Input file is empty.\n");
	} else {
		fseek(input_file, 0, SEEK_SET);
	}
	

	while (!end_of_file) {
		memset(buffer, 0, sizeof(unsigned char) * SIZE_BUF);
		int amount_read = 0;
		amount_read = fread(buffer, sizeof(unsigned char), SIZE_BUF, input_file);
		if (amount_read < SIZE_BUF) {
			end_of_file = 1;
		}

		for (int i = 0; i < amount_read; i++) {
			counter_bits += amount_of_significant_bits[buffer[i]];
	
			bool end_of_block = false;
	
			if (counter_bits >= 64) {
				counter_bits %= 64;
				end_of_block = true;
			}

			if (end_of_block) {
				buffer_for_write[number] = buffer_for_write[number] << (amount_of_significant_bits[buffer[i]] - counter_bits);
				buffer_for_write[number] = buffer_for_write[number] | (huffman_codes_array[buffer[i]] >> counter_bits);
				number++;
				memset(buffer_for_write + number, 0, sizeof(unsigned long));
				buffer_for_write[number] = buffer_for_write[number] | ((huffman_codes_array[buffer[i]] << (64 - counter_bits)) >> (64 - counter_bits));
				amount_of_written++;
			} else {
				buffer_for_write[number] = buffer_for_write[number] << amount_of_significant_bits[buffer[i]];
				buffer_for_write[number] = buffer_for_write[number] | huffman_codes_array[buffer[i]];
			}
		}
	
		if (end_of_file) {
			fwrite(buffer_for_write, sizeof(unsigned long), number + 1, output_file);
			fflush(output_file);
			continue;
		}
	
		if (number >= SIZE_BUF_FOR_WRITE - 3) {
			fwrite(buffer_for_write, sizeof(unsigned long), number, output_file);
			fflush(output_file);
			unsigned long temp = buffer_for_write[number];
			memset(buffer_for_write, 0, sizeof(unsigned long) * SIZE_BUF_FOR_WRITE);
			buffer_for_write[0] = temp;
			number = 0;
		}
	}

	fseek(output_file, 0, SEEK_SET);
	fwrite(&amount_of_written, sizeof(int), 1, output_file);
	fwrite(&counter_bits, sizeof(int), 1, output_file);
	fwrite(huffman_codes_array, sizeof(unsigned long), SIZE_TABLE, output_file);
	fwrite(amount_of_significant_bits, sizeof(unsigned int), SIZE_TABLE, output_file);

	fclose(output_file);
	fclose(input_file);
}

/**
 * @brief Start point to encode file

 * Function call other functions to encoding input file
 * @param input_file the name of input file
 * @param output_file the name of output file
 */
void encode(char* input_file, char* output_file)
{
	clock_t start_time = clock();
	
	int array_freq[SIZE_TABLE] = {0};				// array for frequency of symbols in input file
	heap_node array_heap_nodes[SIZE_TABLE * 3];			// array of nodes for building huffman tree					
	unsigned long huffman_codes_array[SIZE_TABLE] = {0};		// array for huffman codes
	unsigned int amount_of_significant_bits[SIZE_TABLE] = {0};	// array for amounts of significant bits

	if (get_frequency(input_file, array_freq) == -1) {								
		fprintf(stderr, "Error of openening input file.\n");
		return;
	}
	heap* h = create_heap(SIZE_TABLE);								
	get_huffman_tree(h, array_heap_nodes, array_freq);						// build huffman tree (linkes)
	painting_huffman_tree(h, huffman_codes_array, amount_of_significant_bits);			// get huffman codes and amounts of significant bits

	write_code_in_file(input_file, output_file, huffman_codes_array, amount_of_significant_bits);	// coding symbols and writing code in output file

	delete_heap(h);
	
	clock_t end_time = clock();
	
	printf("time of encoding: \t\t%lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);
}
