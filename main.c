/**
 * @file main.c
 * @author Ivan Matsiash
 * @brief Start file, for working archiver
 *
 * This file checked input arguments and calling necessary function (encode or decode).  
 * @copyright Copyright (c) 2017, Ivan Matsiash / FAMCS BSU, Epam
 */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"

/**
 * @brief Start point of execution application

 * Function gets argument of command line and call necessaries functions
 * @param argc amount of arguments
 * @param argv pointer to array of arguments
 * @return 1 in a case of error, 0 in a case of success
 */
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
