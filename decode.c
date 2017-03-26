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

unsigned long get_max_huffman_code(unsigned long *huffman_codes)
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

void createHashTable(element_st *hash_table, unsigned long *huffman_codes, unsigned int *amount_of_bits, unsigned long max_huff_code)
{
	for (int i = 0; i < max_huff_code; i++)
	{
		hash_table[i].symbol = -1;
		hash_table[i].amount_of_bits = 0;
		hash_table[i].huffman_code = 0;
		hash_table[i].next = NULL;
	}
	
	unsigned long counterForCollisions = max_huff_code / 2;	
	
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
	FILE *out = fopen(output_file, "wb");
	
	int amount = 0;
	int amount_bits_in_last_var = 0;
	int count_buffer_for_write = 0;
	
	unsigned char buffer_for_write[SIZE_BUF_FOR_WRITE];
	unsigned long huffman_codes[SIZE_TABLE];
	unsigned int amount_of_bits[SIZE_TABLE];
	unsigned long code[SIZE_BUF_FOR_READ];
	
	fread(&amount, sizeof(int), 1, file);					// amount of blocks
	fread(&amount_bits_in_last_var, sizeof(int), 1, file);			// amount significant bits in last block
	fread(huffman_codes, sizeof(unsigned long), SIZE_TABLE, file);		// array with huffman codes
	fread(amount_of_bits, sizeof(unsigned int), SIZE_TABLE, file);		// array with amount of significant bits

	unsigned long max_huff_code = get_max_huffman_code(huffman_codes) * 3;	// maximum value of huffman code * 3

	element_st *hash_table = (element_st *)malloc(sizeof(element_st) * max_huff_code);

	if (hash_table == NULL) {
		printf("%s\n", "INVALID ALLOC MEMORY");
		return;
	}

	createHashTable(hash_table, huffman_codes, amount_of_bits, max_huff_code);
	
	fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, file);		// read code from input file
	
	int counter = 1, counter_of_last = 0, symbol = 0, success_bit = 0, count = 0, count_global = 0, end_of_block = 0;
	unsigned long tempCode = 0, last_code = 0;

	int shift_last_block_flag = 1;
	do
	{
		if (count_global == amount - 1 && shift_last_block_flag == 1)
		{
			code[count] = code[count] << (64 - amount_bits_in_last_var);
			shift_last_block_flag = 0;
		}
		tempCode = 0;
		if (end_of_block)
		{
			tempCode |= last_code;
			tempCode = tempCode << counter;
			success_bit = 0;
		}
		else
		{
			counter_of_last = 0;
		}

		tempCode |= ((code[count] << success_bit) >> (64 - counter));
		symbol = binary_search(tempCode, counter + counter_of_last, hash_table);
		if (symbol == -1)
		{

			if ((counter + success_bit) == 64)
			{
				end_of_block = 1;
				last_code = ((code[count] << success_bit) >> (64 - counter));
				counter_of_last = counter;
				counter = 0;
				count_global++;
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
			end_of_block = 0;
			if ((counter - 1) + success_bit == 64)
			{
				count++;
				count_global++;
				success_bit = 0;
				if (count == SIZE_BUF_FOR_READ)
				{
					memset(code, 0, sizeof(unsigned long) * SIZE_BUF_FOR_READ);
					fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, file);
					count = 0;
				}
			}
			if (count_global == amount - 1 && success_bit == amount_bits_in_last_var)
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
