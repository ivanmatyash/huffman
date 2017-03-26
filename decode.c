#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define SIZE_TABLE 256
#define SIZE_BUF_FOR_READ 10000
#define SIZE_BUF_FOR_WRITE 4096

typedef struct element{
	int symbol;
	unsigned int amount_of_bits;
	unsigned long huffman_code;
	struct element* next;
} element_st;

int binary_search(unsigned long code, unsigned int amount_of_bits, element_st *HASH_TABLE)
{
	
	if (HASH_TABLE[code].amount_of_bits == 0)
	{
		return -1;
	}	

	if (HASH_TABLE[code].symbol != -1)
	{
		element_st temp = HASH_TABLE[code];
		do
		{
	
			int t;
			if(temp.amount_of_bits == amount_of_bits)
			{
				return temp.symbol;
			}
			else
			{
				if (temp.next == NULL)
				{
					return -1;
				}
				else
				{
					temp = *(temp.next);
				}
			}
		}while(1);
	}
	else
	{
		return -1;
	}	
}

unsigned long getMaxHuffmanCode(unsigned long *HUFFMAN_CODES)
{
	unsigned long max = 0;
	for (int i = 0; i < SIZE_TABLE; i++)
	{
		if (HUFFMAN_CODES[i] > max)
		{
			max = HUFFMAN_CODES[i];
		}
	}

	return max;
}

void createHashTable(element_st *HASH_TABLE, unsigned long *HUFFMAN_CODES, unsigned int *AMOUNT_OF_BITS, unsigned long maxHuffCode)
{
	for (int i = 0; i < maxHuffCode; i++)
	{
		HASH_TABLE[i].symbol = -1;
		HASH_TABLE[i].amount_of_bits = 0;
		HASH_TABLE[i].huffman_code = 0;
		HASH_TABLE[i].next = NULL;
	}
	
	unsigned long counterForCollisions = maxHuffCode / 2;	
	
	for (int i = 0; i < SIZE_TABLE; i++)
	{
		if (AMOUNT_OF_BITS[i] != 0)
		{
			if (HASH_TABLE[HUFFMAN_CODES[i]].symbol == -1)
			{
				HASH_TABLE[HUFFMAN_CODES[i]].symbol = i;
				HASH_TABLE[HUFFMAN_CODES[i]].amount_of_bits = AMOUNT_OF_BITS[i];
				HASH_TABLE[HUFFMAN_CODES[i]].huffman_code = HUFFMAN_CODES[i];
				HASH_TABLE[HUFFMAN_CODES[i]].next = NULL;
			}
		
			else
			{	HASH_TABLE[counterForCollisions] = HASH_TABLE[HUFFMAN_CODES[i]];
				HASH_TABLE[HUFFMAN_CODES[i]].symbol = i;
				HASH_TABLE[HUFFMAN_CODES[i]].amount_of_bits = AMOUNT_OF_BITS[i];
				HASH_TABLE[HUFFMAN_CODES[i]].huffman_code = HUFFMAN_CODES[i];
				HASH_TABLE[HUFFMAN_CODES[i]].next = &HASH_TABLE[counterForCollisions];
				counterForCollisions++;
			}
		} 
	}



}

void parse_code(char* input_file, char* output_file)
{

	unsigned char buffer_for_write[SIZE_BUF_FOR_WRITE];


	FILE * file = fopen(input_file, "rb");

	int amount = 0;
	int amountBitsInLastVar = 0;
	int countBufferForWrite = 0;

	unsigned long HUFFMAN_CODES[SIZE_TABLE];
	unsigned int AMOUNT_OF_BITS[SIZE_TABLE];
	fread(&amount, sizeof(int), 1, file);
	fread(&amountBitsInLastVar, sizeof(int), 1, file);
	fread(HUFFMAN_CODES, sizeof(unsigned long), SIZE_TABLE, file);
	fread(AMOUNT_OF_BITS, sizeof(unsigned int), SIZE_TABLE, file);


	unsigned long maxHuffCode = getMaxHuffmanCode(HUFFMAN_CODES) * 3;
	element_st *HASH_TABLE = (element_st *)malloc(sizeof(element_st) * maxHuffCode);
	if (HASH_TABLE == NULL)
		printf("%s\n", "INVALID MEMORY");
	createHashTable(HASH_TABLE, HUFFMAN_CODES, AMOUNT_OF_BITS, maxHuffCode);
	
	unsigned long code[SIZE_BUF_FOR_READ];
	fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, file);
	
	FILE *out = fopen(output_file, "wb");

	int counter = 1, counterOfLast = 0;
	int symbol = 0;
	unsigned long tempCode = 0, last_code = 0;
	int success_bit = 0;
	int count = 0, countGlobal = 0;

	int endOfStruct = 0;
	int flag1 = 1;
	do
	{
		if (countGlobal == amount - 1 && flag1 == 1)
		{
			code[count] = code[count] << (64 - amountBitsInLastVar);
			flag1 = 0;
		}
		tempCode = 0;
		if (endOfStruct)
		{
			tempCode |= last_code;
			tempCode = tempCode << counter;
			success_bit = 0;
		}
		else
		{
			counterOfLast = 0;
		}

		tempCode |= ((code[count] << success_bit) >> (64 - counter));
		symbol = binary_search(tempCode, counter + counterOfLast, HASH_TABLE);
		if (symbol == -1)
		{

			if ((counter + success_bit) == 64)
			{
				endOfStruct = 1;
				last_code = ((code[count] << success_bit) >> (64 - counter));
				counterOfLast = counter;
				counter = 0;
				countGlobal++;
				count++;
				if (count  == SIZE_BUF_FOR_READ)	
				{
					memset(code, 0, sizeof(unsigned long) * SIZE_BUF_FOR_READ);
					fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, file);
					count = 0;
				}
			}	
			counter++;
			continue;
		}
		else
		{
			buffer_for_write[countBufferForWrite] = symbol;
			countBufferForWrite++;
			if (countBufferForWrite == SIZE_BUF_FOR_WRITE)
			{
				fwrite(buffer_for_write, sizeof(char), SIZE_BUF_FOR_WRITE, out);
				countBufferForWrite = 0;
			}
			success_bit += counter;
			counter = 1;
			endOfStruct = 0;
			if ((counter - 1) + success_bit == 64)
			{
				count++;
				countGlobal++;
				success_bit = 0;
				if (count == SIZE_BUF_FOR_READ)
				{
					memset(code, 0, sizeof(unsigned long) * SIZE_BUF_FOR_READ);
					fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, file);
					count = 0;
				}
			}
			if (countGlobal == amount - 1 && success_bit == amountBitsInLastVar)
			{
				fwrite(buffer_for_write, sizeof(unsigned char), countBufferForWrite, out);
				break;
			}
		}
	} while(1);

	fflush(out);	

	fclose(out);
	fclose(file);
	free(HASH_TABLE);
}

void decode(char* input_file, char* output_file)
{
	clock_t start_time = clock();
	parse_code(input_file, output_file);

	clock_t end_time = clock();
	printf("time of decoding: %lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);

	struct stat st;
	stat(output_file, &st);
	int size = st.st_size;
	printf("Size of decompress-file: %d\n", size);
}
