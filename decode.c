#include <stdio.h>
#include <stdlib.h>

#define SIZE_TABLE 256

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

int binary_search(unsigned long code, element_st* elementArray, unsigned int amount_of_bits)
{
	
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

void parse_code(unsigned long *code, int size, int amountBitsInLastVar, element_st* elementArray)
{

	FILE *out = fopen("decode.txt", "wb");

	/*for (int i = 0; i < SIZE_TABLE; i++)
	{
		printf("Sym: %d code = %lu  bits = %u\n", elementArray[i].symbol, elementArray[i].huffman_code, elementArray[i].amount_of_bits);
	}*/	

	code[size - 1]= code[size - 1] << (64 - amountBitsInLastVar);
	int counter = 1, counterOfLast = 0;
	int symbol = 0;
	unsigned long tempCode = 0, last_code = 0;
	int success_bit = 0;
	int count = 0, t = 0;

	int endOfStruct = 0;

	//int ggg = 0;
	do
	{
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
		symbol = binary_search(tempCode, elementArray, counter + counterOfLast);
		if (symbol == -1)
		{

			if ((counter + success_bit) == 64)
			{
				endOfStruct = 1;
				last_code = ((code[count] << success_bit) >> (64 - counter));
				counterOfLast = counter;
				counter = 0;
				count++;
			}	
			counter++;
			//scanf("%d", &ggg);
			continue;
		}
		else
		{
			fwrite(&symbol, sizeof(unsigned char), 1, out);
			//printf("%c\n", symbol);
			success_bit += counter;
			counter = 1;
			//printf("suc bits: %d\n", success_bit);
			endOfStruct = 0;
			if ((counter - 1) + success_bit == 64)
			{
				count++;
				success_bit = 0;
			}
			//scanf("%d", &t);
			if (count == size - 1 && success_bit == amountBitsInLastVar)
			{
				break;
			}
		}
	} while(1);
//	printf("%d\n", sizeof(tempCode));

	fclose(out);
}

int main(void)
{
	element_st elementArray[SIZE_TABLE];	

	FILE * file = fopen("out.bin", "rb");

	int amount = 0;
	int amountBitsInLastVar = 0;
	unsigned long HUFFMAN_CODES[SIZE_TABLE];
	unsigned int AMOUNT_OF_BITS[SIZE_TABLE];
	fread(&amount, sizeof(int), 1, file);
	fread(&amountBitsInLastVar, sizeof(int), 1, file);
	fread(HUFFMAN_CODES, sizeof(unsigned long), SIZE_TABLE, file);
	fread(AMOUNT_OF_BITS, sizeof(unsigned int), SIZE_TABLE, file);
	//showArray(AMOUNT_OF_BITS, HUFFMAN_CODES, SIZE_TABLE);

	//printf("amount = %d\n", amount);

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
	unsigned long buffer_zip[amount];
	fread(buffer_zip, sizeof(unsigned long), amount, file);
	parse_code(buffer_zip, amount, amountBitsInLastVar, elementArray);
	

	//for (int i = 0; i < amount; i++)	
	//	printf("%lu\n", buffer_zip[i]);
	
	fclose(file);
	return 0;
}
