#include "encode.h"
#include "heap.h"
#include <math.h>
#include <string.h>

#define SIZE_BUF 100
#define SIZE_TABLE 256
#define SIZE_BUF_FOR_WRITE 5

void showArray(unsigned int* array, unsigned long *ar,  int size)
{
	printf("\n\nSHOW ARRAY CHAR\n\n");
	for (int i = 0; i < size; i++)
	{
		printf("%d: %lu - %d\n", i, ar[i], array[i]);
	}
}

void showArrayInt(int* array, int size)
{
	printf("\n\nSHOW ARRAY INT\n\n");
	for (int i = 0; i < size; i++)
	{
		printf("%d: %d\n", i, array[i]);
	}
}

void updateFreq(int* array_f, unsigned char *buffer)
{
	for (int i = 0; i < SIZE_BUF; i++)
	{
		if (buffer[i] != 0)
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
		if (fread(buffer, 1, SIZE_BUF, file) < SIZE_BUF)
		{
			endOfFile = 0;
		}
		updateFreq(array_f, buffer);
	}
	fclose(file);
	
}

void createHuffmanTree(heap* h, heapNode arrayHeapNodes[SIZE_TABLE * 3])
{
	int counter = 0;
	while(h->curSize >1)
	{
		arrayHeapNodes[counter] = remove_min_node_heap(h);
		counter++;
		arrayHeapNodes[counter] = remove_min_node_heap(h);
//		printf("%d - %c - %d: %d - %c - %d\n\n", arrayHeapNodes[counter-1].priority, arrayHeapNodes[counter-1].value,arrayHeapNodes[counter-1].value, arrayHeapNodes[counter].priority, arrayHeapNodes[counter].value, arrayHeapNodes[counter].value);
		insert_node_heap(h, arrayHeapNodes[counter-1].priority + arrayHeapNodes[counter].priority, arrayHeapNodes[counter-1].value + arrayHeapNodes[counter].value, &arrayHeapNodes[counter-1], &arrayHeapNodes[counter]);
		counter++;
	}	
}

void getHuffmanTree(heap* h, heapNode arrayHeapNodes[SIZE_TABLE * 3], int array_f[SIZE_TABLE])
{
	for (int i = 0; i < SIZE_TABLE; i++)
	{
		if (array_f[i] != 0 && i != '\n')
		{
			insert_node_heap(h, array_f[i], i, NULL, NULL);
		}

	}
	//printf("cur size = %d | %d", h->curSize, h->maxSize);
	createHuffmanTree(h, arrayHeapNodes);
}

void paintingHuffmanTree(heapNode *root, unsigned long HUFFMAN_CODES_ARRAY[SIZE_TABLE], unsigned int AMOUNT_OF_SIGN_BITS[SIZE_TABLE])
{
	
	if (root->right != NULL)
	{
		(root->right)->huffmanCode = root->huffmanCode << 1;
		(root->right)->huffmanCode = (root->right)->huffmanCode | 1;
		(root->right)->amountOfSignificantBits = root->amountOfSignificantBits + 1;
		paintingHuffmanTree(root->right, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);
	}

	if (root->left != NULL)
	{
		(root->left)->huffmanCode = root->huffmanCode << 1;
		(root->left)->amountOfSignificantBits = root->amountOfSignificantBits + 1;
		paintingHuffmanTree(root->left, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);
	}

	if (root->left == NULL && root->right == NULL)
	{
		HUFFMAN_CODES_ARRAY[root->value] = root->huffmanCode;
		AMOUNT_OF_SIGN_BITS[root->value] = root->amountOfSignificantBits;
	}

}

void writeCodeInFile(char* fileName, unsigned long HUFFMAN_CODES_ARRAY[SIZE_TABLE], unsigned int AMOUNT_OF_SIGN_BITS[SIZE_TABLE])
{
	unsigned long bufferForWrite[SIZE_BUF_FOR_WRITE] = {0};	
	unsigned char buffer[SIZE_BUF] = {0};
	int endOfFile = 1;
	FILE* file = fopen(fileName, "rb");
	FILE *fileOut = fopen("out.bin", "wb");
	fseek(fileOut, sizeof(int) + sizeof(unsigned long) * SIZE_TABLE + sizeof(unsigned int) * SIZE_TABLE, SEEK_SET); // place for amount of long variables + codes + amount of bits
	int number = 0, counterBits = 0, amount_of_w = 1;
	while (endOfFile)
	{
		memset(buffer, 0, sizeof(unsigned char) * SIZE_BUF);
		int amount_read = fread(buffer, 1, SIZE_BUF, file);
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
			continue;
		}
	
		if (number >= SIZE_BUF_FOR_WRITE - 3)
		{
			for (int i = 0; i < number; i++)
				printf("i: %d - %lu\n", i, bufferForWrite[i]);
			fwrite(bufferForWrite, sizeof(unsigned long), number, fileOut);
			bufferForWrite[0] = bufferForWrite[number];
			memset(bufferForWrite + 1, 0, sizeof(unsigned long) * number + 5);
			number = 0;
		}
	}
	fseek(fileOut, 0, SEEK_SET);
	fwrite(&amount_of_w, sizeof(int), 1, fileOut);
	fwrite(HUFFMAN_CODES_ARRAY, sizeof(unsigned long), SIZE_TABLE, fileOut);
	fwrite(AMOUNT_OF_SIGN_BITS, sizeof(unsigned int), SIZE_TABLE, fileOut);
	fclose(fileOut);
	fclose(file);

	printf("amount = %d\n", amount_of_w);
	for (int i = 0; i <= number; i++)
	{
		printf("%lu\n", bufferForWrite[i]);
	}

	printf("amount bits: %d\n", counterBits);
}

void encode(char* fileName)
{
	int array_f[SIZE_TABLE] = {0};
	getFrequency(fileName, array_f);
	heap* h = create_heap(SIZE_TABLE);
	heapNode arrayHeapNodes[SIZE_TABLE * 3];

//	showArrayInt(array_f, SIZE_TABLE);
	
	getHuffmanTree(h, arrayHeapNodes, array_f);
		
	heapNode root = remove_min_node_heap(h);

	unsigned long HUFFMAN_CODES_ARRAY[SIZE_TABLE];
	unsigned int AMOUNT_OF_SIGN_BITS[SIZE_TABLE];

	paintingHuffmanTree(&root, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);

	writeCodeInFile(fileName, HUFFMAN_CODES_ARRAY, AMOUNT_OF_SIGN_BITS);

//	showArray(AMOUNT_OF_SIGN_BITS, HUFFMAN_CODES_ARRAY, SIZE_TABLE);
	delete_heap(h);
}
