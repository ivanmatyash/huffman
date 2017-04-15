/**
 * @file decode.c
 * @author Ivan Matsiash
 * @brief Implementation of functions of decoding files
 *
 * This file contains implementations of functions, that used in processes of decoding files.
 * @copyright Copyright (c) 2017, Ivan Matsiash / FAMCS BSU, Epam
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define SIZE_TABLE 256
#define SIZE_BUF_FOR_READ 10000
#define SIZE_BUF_FOR_WRITE 4096

/**
 * Structure for hash table elements
 */
typedef struct element{
/**
 * @name fields of elements of hash table
 */
 /*@{*/ 
	int symbol; /**< code symbol (char) (int for returning -1 in case of error)*/
	unsigned int amount_of_bits; /**< amount of significant bits for this node*/
	unsigned long huffman_code; /**< huffman code for this node*/	
	struct element* next; /**< link at the next node in case of collision*/
} element_st;


/**
 * @brief Function search huffman code int hash table

 * Function search symbol in hash table by this huffman code
 * @param code huffman code of symbol
 * @param amount_of_bits amount of significant bits for this symbol
 * @param huffman_codes_array pointer to array of huffman codes
 * @param hash_table pointer to hash table
 * @return symbol by his huffman code
 */
int search_in_hash_table(unsigned long code, unsigned int amount_of_bits, element_st *hash_table)
{	
	if (hash_table[code].amount_of_bits == 0) {				// if this position in hash table is free
		return -1;
	}	

	if (hash_table[code].symbol != -1) {					// if this position in hash table is not empty
		element_st temp = hash_table[code];				// get structure of this position in hash table
		do {
			if (temp.amount_of_bits == amount_of_bits) { 		// if huffman code and amount of bits are equals - return symbol
				return temp.symbol;
			} else {						// if huffman code is equals but amount of bits - difference
				if (temp.next == NULL) {			// if with this huffman code only one node
					return -1;
				} else {
					temp = *(temp.next);			// go to the next node in list with this huffman code
				}
			}
		} while(1);
	} else {
		return -1;							// return -1 if huffman codes are not equals
	}	
}

/**
 * @brief Function search maximum value in array of huffman codes

 * Function get maximum value of huffman codes from array of huffman codes
 * @param huffman_codes pointer to array of huffman codes
 * @return maximum huffman code
 */
unsigned long get_max_huffman_code(unsigned long *huffman_codes)
{
	unsigned long max = 0;
	for (int i = 0; i < SIZE_TABLE; i++) {
		if (huffman_codes[i] > max) {
			max = huffman_codes[i];
		}
	}
	return max;
}

/**
 * @brief Function create hash table for huffman codes for decoding

 * Function create hash table by array of huffman codes
 * @param hash_table pointer to hash table
 * @param huffman_codes pointer to array of huffman codes
 * @param amount_of_bits pointer to array of amount of significant bits
 * @param max_huff_code maximum value of huffman codes
 */
void create_hash_table(element_st *hash_table, unsigned long *huffman_codes, unsigned int *amount_of_bits, unsigned long max_huff_code)
{
	for (int i = 0; i < max_huff_code; i++)
	{
		hash_table[i].symbol = -1;
		hash_table[i].amount_of_bits = 0;
		hash_table[i].huffman_code = 0;
		hash_table[i].next = NULL;
	}

	unsigned long counter_for_collisions = max_huff_code / 2;	
	
	for (int i = 0; i < SIZE_TABLE; i++) {
		if (amount_of_bits[i] != 0) {							// if not zero symbol in array
			if (hash_table[huffman_codes[i]].symbol == -1) {			// if this place in hash table free
				hash_table[huffman_codes[i]].symbol = i;
				hash_table[huffman_codes[i]].amount_of_bits = amount_of_bits[i];
				hash_table[huffman_codes[i]].huffman_code = huffman_codes[i];
				hash_table[huffman_codes[i]].next = NULL;
			} else {									// if at this play already exist symbol
				hash_table[counter_for_collisions] = hash_table[huffman_codes[i]];	// replace old symbol in the end of array
				hash_table[huffman_codes[i]].symbol = i;
				hash_table[huffman_codes[i]].amount_of_bits = amount_of_bits[i];
				hash_table[huffman_codes[i]].huffman_code = huffman_codes[i];
				hash_table[huffman_codes[i]].next = &hash_table[counter_for_collisions];
				counter_for_collisions++;						// link to the end of array
			}
		} 
	}
}

/**
 * @brief Function decoding input file and write result in output file

 * Function read block form input file to buffer, decoding this and write result in output file
 * @param input_file_name the name of input file
 * @param output_file_name the name of output file
 */
void parse_code(char* input_file_name, char* output_file_name)
{
	FILE *input_file = fopen(input_file_name, "rb");
	FILE *output_file = fopen(output_file_name, "wb");
	
	int amount = 0;
	int amount_bits_in_last_var = 0;
	int count_buffer_for_write = 0;
	
	unsigned char buffer_for_write[SIZE_BUF_FOR_WRITE];			// buffer for writting symbols in output file
	unsigned long huffman_codes[SIZE_TABLE];				// array with huffman codes
	unsigned int amount_of_bits[SIZE_TABLE];				// array with amounts of significant bits
	unsigned long code[SIZE_BUF_FOR_READ];					// read info from input file
	
	fread(&amount, sizeof(int), 1, input_file);				// amount of blocks
	if (amount == 0) {
		fclose(input_file);	
		fclose(output_file);
		return;
	}
	fread(&amount_bits_in_last_var, sizeof(int), 1, input_file);		// amount significant bits in last block
	fread(huffman_codes, sizeof(unsigned long), SIZE_TABLE, input_file);	// array with huffman codes
	fread(amount_of_bits, sizeof(unsigned int), SIZE_TABLE, input_file);	// array with amount of significant bits

	unsigned long max_huff_code = get_max_huffman_code(huffman_codes) * 3;	// maximum value of huffman code * 3

	element_st *hash_table = (element_st *)malloc(sizeof(element_st) * max_huff_code);

	if (hash_table == NULL) {
		printf("%s\n", "INVALID ALLOC MEMORY");
		return;
	}

	create_hash_table(hash_table, huffman_codes, amount_of_bits, max_huff_code);
	
	fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, input_file);	// read code from input file
	
	int counter = 1, counter_of_last = 0, symbol = 0, success_bit = 0, count = 0, count_global = 0, end_of_block = 0, shift_last_block_flag = 1;
	unsigned long temp_code = 0, last_code = 0;

	do {
		if (count_global == amount - 1 && shift_last_block_flag == 1) {	// if this last block - you must to shift <- bits 000000xxxx -> xxxx000000000
			code[count] <<= (64 - amount_bits_in_last_var);
			shift_last_block_flag = 0;
		}
	
		temp_code = 0; 				// read bit after bit
	
		if (end_of_block) {			// if end of block - replace old bits in new struct
			temp_code |= last_code;
			temp_code <<= counter;
			success_bit = 0;
		} else {
			counter_of_last = 0;
		}

		temp_code |= ((code[count] << success_bit) >> (64 - counter));				// in temp code - may be symbol

		symbol = search_in_hash_table(temp_code, counter + counter_of_last, hash_table);	// check, symbol or not
		
		if (symbol == -1) {									// if not a symbol
			if ((counter + success_bit) == 64) {
				end_of_block = 1;
				last_code = ((code[count] << success_bit) >> (64 - counter));		// if end of block - remember last bits
				counter_of_last = counter;
				counter = 0;
				count_global++;
				count++;
				
				if (count  == SIZE_BUF_FOR_READ) {					// if the end of buffer - red new info from input file
					memset(code, 0, sizeof(unsigned long) * SIZE_BUF_FOR_READ);
					fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, input_file);
					count = 0;
				}
			}	
			counter++;
			continue;
		} else {										// if a symbol
			buffer_for_write[count_buffer_for_write] = symbol;				// write in buffer for read symbol
			count_buffer_for_write++;
			if (count_buffer_for_write == SIZE_BUF_FOR_WRITE){				// if buffer is fully - write this in file
				fwrite(buffer_for_write, sizeof(char), SIZE_BUF_FOR_WRITE, output_file);
				count_buffer_for_write = 0;
			}
	
			success_bit += counter;
			counter = 1;
			end_of_block = 0;
			
			if ((counter - 1) + success_bit == 64) {					// if bits was a last in block
				count++;
				count_global++;
				success_bit = 0;
				if (count == SIZE_BUF_FOR_READ) {
					memset(code, 0, sizeof(unsigned long) * SIZE_BUF_FOR_READ);
					fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, input_file);
					count = 0;
				}
			}
			if (count_global == amount - 1 && success_bit == amount_bits_in_last_var) {			// if this is a last block in over - write this in file
				fwrite(buffer_for_write, sizeof(unsigned char), count_buffer_for_write, output_file);
				break;
			}
		}
	} while(1);

	fflush(output_file);	
	fclose(output_file);
	fclose(input_file);
	free(hash_table);
}

/**
 * @brief Start point to decode file

 * Function call other functions to decoding input file
 * @param input_file the name of input file
 * @param output_file the name of output file
 */
void decode(char* input_file, char* output_file)
{
	clock_t start_time = clock();
	
	parse_code(input_file, output_file);
	
	clock_t end_time = clock();
	
	printf("time of decoding: \t\t%lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);
	struct stat st;
	stat(output_file, &st);
	int size = st.st_size;
	printf("Size of decompress-file: \t%d\n", size);
}
