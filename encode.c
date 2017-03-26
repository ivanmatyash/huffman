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
	for (int i = 0; i < size; i++)
	{
		if (array[i] != 0)
		printf("%d: %lu - %d\n", i, ar[i], array[i]);
	}
}

void updateFreq(int* array_f, unsigned char *buffer)
{
	for (int i = 0; i < SIZE_BUF; i++)
	{
			array_f[buffer[i]]++;
	}
}

void getFrequency(char* fileName, int *array_f)
{
	unsigned char buffer[SIZE_BUF] = {0};
	int endOfFile = 1;
	FILE* file = fopen(fileName, "rb");
	while (endOfFile)
	{
		memset(buffer, 0, sizeof(unsigned char) * SIZE_BUF);
		if (fread(buffer, sizeof(unsigned char), SIZE_BUF, file) < SIZE_BUF)
		{
			endOfFile = 0;
		}
		updateFreq(array_f, buffer);
	}
	fclose(file);
	
}

void createHuffmanTree(heap* h, heap_node arrayHeapNodes[SIZE_TABLE * 3])
{
	int counter = 0;
	while(h->cur_size >1)
	{
		arrayHeapNodes[counter] = remove_min_node_heap(h);
		counter++;
		arrayHeapNodes[counter] = remove_min_node_heap(h);
//		printf("%d - %c - %d: %d - %c - %d\n\n", arrayHeapNodes[counter-1].priority, arrayHeapNodes[counter-1].value,arrayHeapNodes[counter-1].value, arrayHeapNodes[counter].priority, arrayHeapNodes[counter].value, arrayHeapNodes[counter].value);
		insert_node_heap(h, arrayHeapNodes[counter-1].priority + arrayHeapNodes[counter].priority, arrayHeapNodes[counter-1].value + arrayHeapNodes[counter].value, &arrayHeapNodes[counter-1], &arrayHeapNodes[counter]);
		counter++;
	}	
}

void getHuffmanTree(heap* h, heap_node arrayHeapNodes[SIZE_TABLE * 3], int array_f[SIZE_TABLE])
{
	for (int i = 0; i < SIZE_TABLE; i++)
	{
		if (array_f[i] != 0)
		{
			insert_node_heap(h, array_f[i], i, NULL, NULL);
		}

	}
	//printf("cur size = %d | %d", h->curSize, h->maxSize);
	createHuffmanTree(h, arrayHeapNodes);
}

void paintingHuffmanTree(heap_node *root, unsigned long HUFFMAN_CODES_ARRAY[SIZE_TABLE], unsigned int AMOUNT_OF_SIGN_BITS[SIZE_TABLE])
{
	
	if (root->right != NULL)
	{
		(root->right)->huffman_code = root->huffman_code << 1;
		(root->right)->huffman_code = (root->right)->huffman_code | 1;
		(root->right)->amount_of_significant_bits = root->amount_of_significant_bits + 1;
		paintingHuffmanTree(root->right, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);
	}

	if (root->left != NULL)
	{
		(root->left)->huffman_code = root->huffman_code << 1;
		(root->left)->amount_of_significant_bits = root->amount_of_significant_bits + 1;
		paintingHuffmanTree(root->left, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);
	}

	if (root->left == NULL && root->right == NULL)
	{
		HUFFMAN_CODES_ARRAY[root->value] = root->huffman_code;
		AMOUNT_OF_SIGN_BITS[root->value] = root->amount_of_significant_bits;
	}

}

void writeCodeInFile(char* input_file, char* output_file, unsigned long HUFFMAN_CODES_ARRAY[SIZE_TABLE], unsigned int AMOUNT_OF_SIGN_BITS[SIZE_TABLE])
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
			counterBits += AMOUNT_OF_SIGN_BITS[buffer[i]];
			bool flag = false;
			if (counterBits >= 64)
			{
				counterBits = counterBits % 64;
				flag = true;
			}
			if (flag)
			{
				bufferForWrite[number] = bufferForWrite[number] << (AMOUNT_OF_SIGN_BITS[buffer[i]] - counterBits);
				bufferForWrite[number] = bufferForWrite[number] | (HUFFMAN_CODES_ARRAY[buffer[i]] >> counterBits);
				number++;
				bufferForWrite[number] = bufferForWrite[number] | ((HUFFMAN_CODES_ARRAY[buffer[i]] << (64 - counterBits)) >> (64 - counterBits));
				amount_of_w++;
					
			}
			else {
				bufferForWrite[number] = bufferForWrite[number] << AMOUNT_OF_SIGN_BITS[buffer[i]];
				bufferForWrite[number] = bufferForWrite[number] | HUFFMAN_CODES_ARRAY[buffer[i]];
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
//			for (int i = 0; i < number; i++)
//				printf("i: %d - %lu\n", i, bufferForWrite[i]);
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
	fwrite(HUFFMAN_CODES_ARRAY, sizeof(unsigned long), SIZE_TABLE, fileOut);
	fwrite(AMOUNT_OF_SIGN_BITS, sizeof(unsigned int), SIZE_TABLE, fileOut);
	fclose(fileOut);
	fclose(file);

	printf("amount = %d\n", amount_of_w);
/*	for (int i = 0; i <= number; i++)
	{
		printf("%lu\n", bufferForWrite[i]);
	}
*/
	printf("amount bits: %d\n", counterBits);
}

void encode(char* input_file, char* output_file)
{
	heap_node *t;
	printf("size = %zu\n", sizeof(unsigned int));
	clock_t start_time = clock();
	int array_f[SIZE_TABLE] = {0};
	getFrequency(input_file, array_f);
	heap* h = create_heap(SIZE_TABLE);
	heap_node arrayHeapNodes[SIZE_TABLE * 3];

//	showArrayInt(array_f, SIZE_TABLE);
	
	getHuffmanTree(h, arrayHeapNodes, array_f);
		
	heap_node root = remove_min_node_heap(h);

	unsigned long HUFFMAN_CODES_ARRAY[SIZE_TABLE];
	unsigned int AMOUNT_OF_SIGN_BITS[SIZE_TABLE];

	if (root.left == NULL && root.right == NULL)
	{
		HUFFMAN_CODES_ARRAY[root.value] = 0;
		AMOUNT_OF_SIGN_BITS[root.value] = 1;
	}
	else
	{
		paintingHuffmanTree(&root, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);
	}

	writeCodeInFile(input_file, output_file, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);

	clock_t end_time = clock();

	printf("time of encoding: %lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);
	struct stat st;
	stat(input_file, &st);
	int size = st.st_size;
	printf("Size of input file: %d\n", size);
//	showArray(AMOUNT_OF_SIGN_BITS, HUFFMAN_CODES_ARRAY, SIZE_TABLE);
	delete_heap(h);
	
}
