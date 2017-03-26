#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"

int main(int argc, char* argv[])
{
	if (argc != 4) {
		fprintf(stderr, "%s\n", "Error! Incorrect number of parameters.\nHint: program ifile [-c|-x] ofile");
		return 1;
	}

	if (!strcmp(argv[2], "-c")) {
		encode(argv[1], argv[3]);
	} else if (!strcmp(argv[2], "-x")) {
		decode(argv[1], argv[3]);
	} else {
		fprintf(stderr, "%s\n", "Error! Wrong arguments.\nHint: program ifile [-c|-x] ofile");
		return 1;
	}
		
	return 0;
}
