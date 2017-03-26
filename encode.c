#include "encode.h"
#include "heap.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define SIZE_BUF 10 
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

void get_frequency(char* file_name, int *array_freq)
{
	FILE* file = fopen(file_name, "rb");
	unsigned char buffer[SIZE_BUF] = {0};
	int end_of_file = 1;

	while (end_of_file) {
		memset(buffer, 0, sizeof(unsigned char) * SIZE_BUF);
		if (fread(buffer, sizeof(unsigned char), SIZE_BUF, file) < SIZE_BUF) {
			end_of_file = 0;
		}
		update_freq(array_freq, buffer);
	}
	fclose(file);
}

void create_huffman_tree(heap* h, heap_node *array_heap_nodes)
{
	int counter = 0;
	while(h->cur_size > 1) {
		array_heap_nodes[counter] = remove_min_node_heap(h);
		counter++;
		array_heap_nodes[counter] = remove_min_node_heap(h);

		int new_priority = array_heap_nodes[counter-1].priority + array_heap_nodes[counter].priority;
		unsigned char new_value = array_heap_nodes[counter-1].value + array_heap_nodes[counter].value;
	
		insert_node_heap(h, new_priority, new_value, &array_heap_nodes[counter-1], &array_heap_nodes[counter]);
		counter++;
	}	
}

void get_huffman_tree(heap* h, heap_node *array_heap_nodes, int *array_freq)
{
	for (int i = 0; i < SIZE_TABLE; i++) {
		if (array_freq[i] != 0) {
			insert_node_heap(h, array_freq[i], i, NULL, NULL);
		}
	}
	create_huffman_tree(h, array_heap_nodes);
}

void painting_huffman_tree(heap_node *root, unsigned long *huffman_codes_array, unsigned int *amount_of_significant_bits)
{
	
	if (root->right != NULL)
	{
		(root->right)->huffman_code = root->huffman_code << 1;
		(root->right)->huffman_code = (root->right)->huffman_code | 1;
		(root->right)->amount_of_significant_bits = root->amount_of_significant_bits + 1;
		painting_huffman_tree(root->right, huffman_codes_array, amount_of_significant_bits);
	}

	if (root->left != NULL)
	{
		(root->left)->huffman_code = root->huffman_code << 1;
		(root->left)->amount_of_significant_bits = root->amount_of_significant_bits + 1;
		painting_huffman_tree(root->left, huffman_codes_array, amount_of_significant_bits);
	}

	if (root->left == NULL && root->right == NULL)
	{
		huffman_codes_array[root->value] = root->huffman_code;
		amount_of_significant_bits[root->value] = root->amount_of_significant_bits;
	}

}

void write_code_in_file(char* input_file, char* output_file, unsigned long huffman_codes_array[SIZE_TABLE], unsigned int amount_of_significant_bits[SIZE_TABLE])
{
	unsigned long bufferForWrite[SIZE_BUF_FOR_WRITE] = {0};	
	unsigned char buffer[SIZE_BUF] = {0};
	int endOfFile = 1;
	FILE* file = fopen(input_file, "rb");
	FILE *fileOut = fopen(output_file, "wb");
	fseek(fileOut, sizeof(int)  + sizeof(int) + sizeof(unsigned long) * SIZE_TABLE + sizeof(unsigned int) * SIZE_TABLE, SEEK_SET); // place for amount of long variables + amount of last bits +  codes + amount of bits
	int number = 0, counterBits = 0, amount_of_w = 1;
	while (endOfFile)
	{
		memset(buffer, 0, sizeof(unsigned char) * SIZE_BUF);
		int amount_read = fread(buffer, sizeof(unsigned char), SIZE_BUF, file);
		if (amount_read < SIZE_BUF)
		{
			endOfFile = 0;
		}
		for (int i = 0; i < amount_read; i++)
		{
			counterBits += amount_of_significant_bits[buffer[i]];
			bool flag = false;
			if (counterBits >= 64)
			{
				counterBits = counterBits % 64;
				flag = true;
			}
			if (flag)
			{
				bufferForWrite[number] = bufferForWrite[number] << (amount_of_significant_bits[buffer[i]] - counterBits);
				bufferForWrite[number] = bufferForWrite[number] | (huffman_codes_array[buffer[i]] >> counterBits);
				number++;
				bufferForWrite[number] = bufferForWrite[number] | ((huffman_codes_array[buffer[i]] << (64 - counterBits)) >> (64 - counterBits));
				amount_of_w++;
					
			}
			else {
				bufferForWrite[number] = bufferForWrite[number] << amount_of_significant_bits[buffer[i]];
				bufferForWrite[number] = bufferForWrite[number] | huffman_codes_array[buffer[i]];
			}
		}
	
		if (!endOfFile)
		{
			fwrite(bufferForWrite, sizeof(unsigned long), number+1, fileOut);
			fflush(fileOut);
			continue;
		}
	
		if (number >= SIZE_BUF_FOR_WRITE - 3)
		{
			fwrite(bufferForWrite, sizeof(unsigned long), number, fileOut);
			fflush(fileOut);
			unsigned long temp = bufferForWrite[number];
			memset(bufferForWrite, 0, sizeof(unsigned long) * SIZE_BUF_FOR_WRITE);
			bufferForWrite[0] = temp;
			number = 0;
		}
	}
	fseek(fileOut, 0, SEEK_SET);
	fwrite(&amount_of_w, sizeof(int), 1, fileOut);
	fwrite(&counterBits, sizeof(int), 1, fileOut);
	fwrite(huffman_codes_array, sizeof(unsigned long), SIZE_TABLE, fileOut);
	fwrite(amount_of_significant_bits, sizeof(unsigned int), SIZE_TABLE, fileOut);
	fclose(fileOut);
	fclose(file);
}

void encode(char* input_file, char* output_file)
{
	clock_t start_time = clock();
	
	int array_freq[SIZE_TABLE] = {0};
	heap_node array_heap_nodes[SIZE_TABLE * 3];
	unsigned long huffman_codes_array[SIZE_TABLE];
	unsigned int amount_of_significant_bits[SIZE_TABLE];

	get_frequency(input_file, array_freq);
	
	heap* h = create_heap(SIZE_TABLE);
	
	get_huffman_tree(h, array_heap_nodes, array_freq);
		
	heap_node root = remove_min_node_heap(h);

	if (root.left == NULL && root.right == NULL)
	{
		huffman_codes_array[root.value] = 0;
		amount_of_significant_bits[root.value] = 1;
	}
	else
	{
		painting_huffman_tree(&root, huffman_codes_array, amount_of_significant_bits);
	}

	write_code_in_file(input_file, output_file, huffman_codes_array, amount_of_significant_bits);

	clock_t end_time = clock();

	printf("time of encoding: %lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);
	struct stat st;
	stat(input_file, &st);
	int size = st.st_size;
	printf("Size of input file: %d\n", size);
	delete_heap(h);
	
}
