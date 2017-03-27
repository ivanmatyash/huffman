#include "encode.h"
#include "heap.h"
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define SIZE_BUF 200 
#define SIZE_TABLE 256
#define SIZE_BUF_FOR_WRITE 500 

void showArray(unsigned int* array, unsigned long *ar,  int size)
{
	printf("\n\nSHOW ARRAY CHAR\n\n");
	for (int i = 0; i < size; i++) {
		if (array[i] != 0)
		printf("%d: %lu - %d\n", i, ar[i], array[i]);
	}
}

void update_freq(int* array_freq, unsigned char *buffer)
{
	for (int i = 0; i < SIZE_BUF; i++) {
		array_freq[buffer[i]]++;
	}
}

int get_frequency(char* file_name, int *array_freq)
{
	FILE* file = fopen(file_name, "rb");
	if (file == NULL) {
		return -1;
	}
	unsigned char buffer[SIZE_BUF] = {0};
	int end_of_file = 1;

	while (end_of_file) {
		memset(buffer, 0, sizeof(unsigned char) * SIZE_BUF);				// clean buffer
		if (fread(buffer, sizeof(unsigned char), SIZE_BUF, file) < SIZE_BUF) {		// read next block from input file
			end_of_file = 0;
		}
		update_freq(array_freq, buffer);
	}
	fclose(file);
	return 0;
}

void create_huffman_tree(heap* h, heap_node *array_heap_nodes)
{
	int counter = 0;
	while(h->cur_size > 1) {
		array_heap_nodes[counter] = remove_min_node_heap(h);
		counter++;
		array_heap_nodes[counter] = remove_min_node_heap(h);							// get 2 minimum-priority nodes from binary heap
	
		int new_priority = array_heap_nodes[counter-1].priority + array_heap_nodes[counter].priority;		// calculate priority for new united node
		unsigned char new_value = array_heap_nodes[counter-1].value + array_heap_nodes[counter].value;		// calculate value for new united node
	
		insert_node_heap(h, new_priority, new_value, &array_heap_nodes[counter-1], &array_heap_nodes[counter]); // insert united node into binary heap
		counter++;
	}	
}

void get_huffman_tree(heap* h, heap_node *array_heap_nodes, int *array_freq)
{
	for (int i = 0; i < SIZE_TABLE; i++) {
		if (array_freq[i] != 0) {
			insert_node_heap(h, array_freq[i], i, NULL, NULL);			// make nodes for every symbol and put it into binary heap
		}
	}
	create_huffman_tree(h, array_heap_nodes);
}

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
		amount_of_written = 0;					// amount of writting blocks = 0
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

void encode(char* input_file, char* output_file)
{
	clock_t start_time = clock();
	
	int array_freq[SIZE_TABLE] = {0};			// array for frequency of symbols in input file
	heap_node array_heap_nodes[SIZE_TABLE * 3];		// array of nodes for building huffman tree					
	unsigned long huffman_codes_array[SIZE_TABLE];		// array for huffman codes
	unsigned int amount_of_significant_bits[SIZE_TABLE];	// array for amounts of significant bits

	if (get_frequency(input_file, array_freq) == -1) {								// get frequency of symbols in input file
		fprintf(stderr, "Error of openening input file.\n");
		return;
	}
	heap* h = create_heap(SIZE_TABLE);								// create heap for huffman tree	
	get_huffman_tree(h, array_heap_nodes, array_freq);						// build huffman tree (linkes)
	painting_huffman_tree(h, huffman_codes_array, amount_of_significant_bits);			// get huffman codes and amounts of significant bits

	write_code_in_file(input_file, output_file, huffman_codes_array, amount_of_significant_bits);	// coding symbols and writing code in output file

	delete_heap(h);
	
	clock_t end_time = clock();
	
	printf("time of encoding: \t\t%lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);
	struct stat st;
	stat(input_file, &st);
	int size_in = st.st_size;
	printf("Size of input file: \t\t%d\n", size_in);
	stat(output_file, &st);
	int size_out = st.st_size;
	printf("Size of compressed file: \t%d\n", size_out);
	printf("Compression ratio: \t\t%f\n", (float)size_in / size_out);
}
