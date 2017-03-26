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

int binary_search(unsigned long code, unsigned int amount_of_bits, element_st *hash_table)
{
	
	if (hash_table[code].amount_of_bits == 0)
	{
		return -1;
	}	

	if (hash_table[code].symbol != -1)
	{
		element_st temp = hash_table[code];
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

unsigned long getMaxHuffmanCode(unsigned long *huffman_codes)
{
	unsigned long max = 0;
	for (int i = 0; i < SIZE_TABLE; i++)
	{
		if (huffman_codes[i] > max)
		{
			max = huffman_codes[i];
		}
	}

	return max;
}

void createHashTable(element_st *hash_table, unsigned long *huffman_codes, unsigned int *amount_of_bits, unsigned long maxHuffCode)
{
	for (int i = 0; i < maxHuffCode; i++)
	{
		hash_table[i].symbol = -1;
		hash_table[i].amount_of_bits = 0;
		hash_table[i].huffman_code = 0;
		hash_table[i].next = NULL;
	}
	
	unsigned long counterForCollisions = maxHuffCode / 2;	
	
	for (int i = 0; i < SIZE_TABLE; i++)
	{
		if (amount_of_bits[i] != 0)
		{
			if (hash_table[huffman_codes[i]].symbol == -1)
			{
				hash_table[huffman_codes[i]].symbol = i;
				hash_table[huffman_codes[i]].amount_of_bits = amount_of_bits[i];
				hash_table[huffman_codes[i]].huffman_code = huffman_codes[i];
				hash_table[huffman_codes[i]].next = NULL;
			}
		
			else
			{	hash_table[counterForCollisions] = hash_table[huffman_codes[i]];
				hash_table[huffman_codes[i]].symbol = i;
				hash_table[huffman_codes[i]].amount_of_bits = amount_of_bits[i];
				hash_table[huffman_codes[i]].huffman_code = huffman_codes[i];
				hash_table[huffman_codes[i]].next = &hash_table[counterForCollisions];
				counterForCollisions++;
			}
		} 
	}



}

void parse_code(char* input_file, char* output_file)
{
	FILE * file = fopen(input_file, "rb");
	unsigned char buffer_for_write[SIZE_BUF_FOR_WRITE];

	int amount = 0;
	int amount_bits_in_last_var = 0;
	int count_buffer_for_write = 0;

	unsigned long huffman_codes[SIZE_TABLE];
	unsigned int amount_of_bits[SIZE_TABLE];
	fread(&amount, sizeof(int), 1, file);
	fread(&amount_bits_in_last_var, sizeof(int), 1, file);
	fread(huffman_codes, sizeof(unsigned long), SIZE_TABLE, file);
	fread(amount_of_bits, sizeof(unsigned int), SIZE_TABLE, file);


	unsigned long maxHuffCode = getMaxHuffmanCode(huffman_codes) * 3;
	element_st *hash_table = (element_st *)malloc(sizeof(element_st) * maxHuffCode);
	if (hash_table == NULL)
		printf("%s\n", "INVALID MEMORY");
	createHashTable(hash_table, huffman_codes, amount_of_bits, maxHuffCode);
	
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
			code[count] = code[count] << (64 - amount_bits_in_last_var);
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
		symbol = binary_search(tempCode, counter + counterOfLast, hash_table);
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
			buffer_for_write[count_buffer_for_write] = symbol;
			count_buffer_for_write++;
			if (count_buffer_for_write == SIZE_BUF_FOR_WRITE)
			{
				fwrite(buffer_for_write, sizeof(char), SIZE_BUF_FOR_WRITE, out);
				count_buffer_for_write = 0;
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
			if (countGlobal == amount - 1 && success_bit == amount_bits_in_last_var)
			{
				fwrite(buffer_for_write, sizeof(unsigned char), count_buffer_for_write, out);
				break;
			}
		}
	} while(1);

	fflush(out);	

	fclose(out);
	fclose(file);
	free(hash_table);
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
