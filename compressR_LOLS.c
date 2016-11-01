// Process Manager/Parent
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compressR_LOLS.h"

/* MAIN
 *
 * @param1: "./file.txt"
 * @param2: <int>
 * 
 * @output: "./file_txt_LOLSx.txt"
 * "-h" flag for help. 
 */
int main(int argc, char const *argv[]) {

	// Check for correct parameters
	if (argc != 3) {
		fprintf(stderr,"Arguments given are not in valid form.\n");
		return -1;
	}
	
	// Check for -h flag
	if (strcmp(argv[1], "-h") == 0) {
		printf("Enter in \"y86emul\" followed by a space and then the filename\n");
		return 0;
	}
	
	// Open uncompressed file
	FILE* fp;
	fp = fopen(argv[1], "r");	

	// Verify that file was opened
	if (fp == NULL) {
		printf("The filename could not be found\n");
		exit(1);
	}

	// Close uncompressed file
	fclose(fp);
	
	return 0;
}