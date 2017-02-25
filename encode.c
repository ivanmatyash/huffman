#include "encode.h"
#include "heap.h"
#include <math.h>

#define SIZE_BUF 100
#define SIZE_TABLE 256

void showArray(char* array, int size)
{
	printf("\n\nSHOW ARRAY CHAR\n\n");
	for (int i = 0; i < size; i++)
	{
		printf("%d: %d\n", i, array[i]);
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

void updateFreq(int* array_f, unsigned char buffer[SIZE_BUF])
{
	for (int i = 0; i < SIZE_BUF; i++)
	{
		if (buffer[i] != 0)
			array_f[buffer[i]]++;
	}
}

void getFrequency(char* fileName, int array_f[SIZE_TABLE])
{
	unsigned char buffer[SIZE_BUF] = {0};
	int endOfFile = 1;
	FILE* file = fopen(fileName, "rb");
	while (endOfFile)
	{
		if (fread(buffer, 1, 100, file) < SIZE_BUF)
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
		printf("%d - %c - %d: %d - %c - %d\n\n", arrayHeapNodes[counter-1].priority, arrayHeapNodes[counter-1].value,arrayHeapNodes[counter-1].value, arrayHeapNodes[counter].priority, arrayHeapNodes[counter].value, arrayHeapNodes[counter].value);
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

void encode(char* fileName)
{
	int array_f[SIZE_TABLE] = {0};
	getFrequency(fileName, array_f);
	heap* h = create_heap(SIZE_TABLE);
	heapNode arrayHeapNodes[SIZE_TABLE * 3];

	getHuffmanTree(h, arrayHeapNodes, array_f);
		
	heapNode root = remove_min_node_heap(h);
	delete_heap(h);
}
