#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE_TABLE 256
#define SIZE_BUF_FOR_READ 10000
#define SIZE_BUF_FOR_WRITE 4096

typedef struct element{
unsigned char symbol;
unsigned long huffman_code;
unsigned int amount_of_bits;
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

int binary_search(unsigned long code, element_st* elementArray, unsigned int amount_of_bits, int min, int max)
{
	/*
	if (max < min)	
	{
		return -1;
	}

	int mid = min + ((max - min) / 2);

	if (elementArray[mid].huffman_code > code)
	{
		return binary_search(code, elementArray, amount_of_bits, min, mid - 1);
	}
	else if (elementArray[mid].huffman_code < code)
	{
		return binary_search(code, elementArray, amount_of_bits, mid+1, max);
	}
	else 
		return elementArray[mid].symbol;
	*/
	for (int i = 0; i < SIZE_TABLE; i++)
	{
		if ((code == elementArray[i].huffman_code) && (amount_of_bits == elementArray[i].amount_of_bits))
			return elementArray[i].symbol;
	}
	
	return -1;
	/*if (code == 0 && amount_of_bits == 2)
	{
		return 98;
	}
	else if(code == 3 && amount_of_bits == 2)
	{
		return 101;
	}
	else if (code == 4 && amount_of_bits == 3)
	{
		return 32;
	}
	else if (code == 10 && amount_of_bits == 4)
	{
		return 33;
	}
	else if (code == 2 && amount_of_bits == 3)
	{
		return 111;
	}
	else if (code == 3 && amount_of_bits == 3)
	{
		return 112;
	}
	else if (code == 11 && amount_of_bits == 4)
	{
		return 114;
	}
	else return -1;
	*/
}

void parse_code(char* fileName)
{

	unsigned char buffer_for_write[SIZE_BUF_FOR_WRITE];

	element_st elementArray[SIZE_TABLE];	

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

	for (int i = 0; i < SIZE_TABLE; i++)
	{
		elementArray[i].symbol = i;
		elementArray[i].huffman_code = HUFFMAN_CODES[i];
		elementArray[i].amount_of_bits = AMOUNT_OF_BITS[i];
	}
	
	qsort(elementArray, SIZE_TABLE, sizeof(element_st), compare);
	//printf ("%d\n", elementArray[255].huffman_code);
	printf("amount = %d\n", amount);
	printf("bits = %d\n", amountBitsInLastVar);
	unsigned long code[SIZE_BUF_FOR_READ];
	fread(code, sizeof(unsigned long), SIZE_BUF_FOR_READ, file);
	
	printf ("%s\n", "test");
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
		symbol = binary_search(tempCode, elementArray, counter + counterOfLast, 0, SIZE_TABLE - 1);
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

	clock_t start_time = clock();
	parse_code("out.bin");

	clock_t end_time = clock();
	printf("time of decoding: %lf\n",((double) end_time - start_time) / CLOCKS_PER_SEC);

	//for (int i = 0; i < amount; i++)	
	//	printf("%lu\n", buffer_zip[i]);
	
	return 0;
}
