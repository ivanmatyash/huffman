#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE_TABLE 256
#define SIZE_BUF_FOR_READ 10000
#define SIZE_BUF_FOR_WRITE 4096

typedef struct element{
	int symbol;
	unsigned int amount_of_bits;
	unsigned long huffman_code;
	struct element* next;
} element_st;

void showArray(unsigned int* array, unsigned long *ar,  int size)
{
	printf("\n\nSHOW ARRAY CHAR\n\n");
	for (int i = 0; i < size; i++)
	{
		printf("%d: %lu - %d\n", i, ar[i], array[i]);
	}
}

int compare(const void *s1, const void *s2)
{
	element_st *e1 = (element_st *)s1;
	element_st *e2 = (element_st *)s2;
	return e1->huffman_code - e2->huffman_code;
}

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
	
	//element_st ee = *(HASH_TABLE[2].next);
	//printf("hash: symbol - %d, amount - %u, huff - %lu\n", ee.symbol, ee.amount_of_bits, ee.huffman_code);
		
			int t;
			//printf("in while: code = %lu bits = %u\n  sym = %d, huff = %lu, bits = %u\n\n", code, amount_of_bits, temp.symbol, temp.huffman_code, temp.amount_of_bits);
			//scanf("%d", &t);
			if(temp.amount_of_bits == amount_of_bits)
			{
				return temp.symbol;
			}
			else
			{
				//printf("%s", "zahod");
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

	//if (HASH_TABLE[code].symbol != -1 && HASH_TABLE[code].amount_of_bits == amount_of_bits)
	//	return HASH_TABLE[code].symbol;
	//else
	//	return -1;
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

	printf("SIZE HASH: %lu\n", max);
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


	//element_st ee = *(HASH_TABLE[2].next);
	//printf("hash: symbol - %d, amount - %u, huff - %lu\n", ee.symbol, ee.amount_of_bits, ee.huffman_code);
	for (int i = 0; i < maxHuffCode; i++)
	{
		if (HASH_TABLE[i].symbol != -1)
			printf("hash: i = %d, symbol - %d, amount - %u, huff - %lu\n", i, HASH_TABLE[i].symbol, HASH_TABLE[i].amount_of_bits, HASH_TABLE[i].huffman_code);
	}

}

void parse_code(char* fileName)
{

	unsigned char buffer_for_write[SIZE_BUF_FOR_WRITE];

//	element_st elementArray[SIZE_TABLE];	

	FILE * file = fopen(fileName, "rb");

	int amount = 0;
	int amountBitsInLastVar = 0;
	int countBufferForWrite = 0;

	unsigned long HUFFMAN_CODES[SIZE_TABLE];
	unsigned int AMOUNT_OF_BITS[SIZE_TABLE];
	fread(&amount, sizeof(int), 1, file);
	fread(&amountBitsInLastVar, sizeof(int), 1, file);
	fread(HUFFMAN_CODES, sizeof(unsigned long), SIZE_TABLE, file);
	fread(AMOUNT_OF_BITS, sizeof(unsigned int), SIZE_TABLE, file);
	//showArray(AMOUNT_OF_BITS, HUFFMAN_CODES, SIZE_TABLE);

	printf("amount = %d\n", amount);

/*	for (int i = 0; i < SIZE_TABLE; i++)
	{
		elementArray[i].symbol = i;
		elementArray[i].huffman_code = HUFFMAN_CODES[i];
		elementArray[i].amount_of_bits = AMOUNT_OF_BITS[i];
		elementArray[i].next = NULL;
	}
	
	qsort(elementArray, SIZE_TABLE, sizeof(element_st), compare);

	int maxCode = elementArray[SIZE_TABLE - 1].huffman_code;
*/

	unsigned long maxHuffCode = getMaxHuffmanCode(HUFFMAN_CODES) * 3;
	element_st HASH_TABLE[maxHuffCode];
	printf("%s\n", "t");
	createHashTable(HASH_TABLE, HUFFMAN_CODES, AMOUNT_OF_BITS, maxHuffCode);
	
	//printf ("%d\n", elementArray[255].huffman_code);
	printf("amount = %d\n", amount);
	printf("bits = %d\n", amountBitsInLastVar);
	unsigned long code[SIZE_BUF_FOR_READ];
	fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, file);
	
	FILE *out = fopen("decode.txt", "wb");

	/*for (int i = 0; i < SIZE_TABLE; i++)
	{
		printf("Sym: %d code = %lu  bits = %u\n", elementArray[i].symbol, elementArray[i].huffman_code, elementArray[i].amount_of_bits);
	}*/	

	//code[amount - 1]= code[amount - 1] << (64 - amountBitsInLastVar);
	int counter = 1, counterOfLast = 0;
	int symbol = 0;
	unsigned long tempCode = 0, last_code = 0;
	int success_bit = 0;
	int count = 0, countGlobal = 0;

	int endOfStruct = 0;
	int flag1 = 1;
	//int ggg = 0;
	do
	{
		if (countGlobal == amount - 1 && flag1 == 1)
		{
			printf("%s %d\n", "kuku", countGlobal);
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
		//printf("code: %lu\n", tempCode);
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
			//scanf("%d", &ggg);
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
			//printf("%c\n", symbol);
			success_bit += counter;
			counter = 1;
			//printf("suc bits: %d\n", success_bit);
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
			//scanf("%d", &t);
			if (countGlobal == amount - 1 && success_bit == amountBitsInLastVar)
			{
				fwrite(buffer_for_write, sizeof(unsigned char), countBufferForWrite, out);
				break;
			}
		}
	} while(1);
	printf("buf = %d\n", countBufferForWrite);

	fflush(out);	
//	printf("%d\n", sizeof(tempCode));

	fclose(out);
	fclose(file);
}

int main(void)
{
	printf("%zu\n", sizeof(element_st));
	clock_t start_time = clock();
	parse_code("out.bin");

	clock_t end_time = clock();
	printf("time of decoding: %lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);

	//for (int i = 0; i < amount; i++)	
	//	printf("%lu\n", buffer_zip[i]);
	
	return 0;
}
