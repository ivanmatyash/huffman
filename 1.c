#include <stdio.h>

#define SIZE_TABLE 256

void showArray(unsigned int* array, unsigned long *ar,  int size)
{
	printf("\n\nSHOW ARRAY CHAR\n\n");
	for (int i = 0; i < size; i++)
	{
		printf("%d: %lu - %d\n", i, ar[i], array[i]);
	}
}

int main(void)
{

	FILE * file = fopen("out.bin", "rb");

	int amount = 0;

	unsigned long HUFFMAN_CODES[SIZE_TABLE];
	unsigned int AMOUNT_OF_BITS[SIZE_TABLE];
	fread(&amount, sizeof(int), 1, file);
	fread(HUFFMAN_CODES, sizeof(unsigned long), SIZE_TABLE, file);
	fread(AMOUNT_OF_BITS, sizeof(unsigned int), SIZE_TABLE, file);
	printf("amount = %d\n", amount);
	unsigned long t[amount];
//	fseek(file, sizeof(int) + sizeof(unsigned long) * SIZE_TABLE + sizeof(unsigned int) * SIZE_TABLE, SEEK_SET);	
	fread(t, sizeof(unsigned long), amount, file);


	for (int i = 0; i < amount; i++)	
		printf("%lu\n", t[i]);
	
	fclose(file);

	return 0;
}
