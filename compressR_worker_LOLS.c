#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ISALPHA
 * 
 * @param1: ascii integer value
 *
 * Returns true if the input value is an alphabetical character
 */
int isAlpha(int c) {
	if ((c > 64 && c < 91) || (c > 96 && c < 123)) {
		return 1;
	} else {
		return 0;
	}
}

/* COMPRESS
 * 
 * @param1: uncompressed filename
 * @param2: allocated memory for compressed string
 *
 * Compression thread.
 */
void compress(char * filename, char * out_filename, char * str, int chunk, int start) {

	// Open uncompressed file
	FILE* fp;
	fp = fopen(filename, "r");

	// Verify that file was opened
	if (fp == NULL) {
		printf("ERROR: Process %d could not open file: %s\n", getpid(), filename);
		return;
	}

	// Move file handler to start position
	fseek(fp, start, SEEK_SET);

	// Copy chunk in to string
	int i, k;
	for (i = 0, k = 0; i < chunk; i++) {

		// Get character from file
		char c = fgetc(fp);

		// Check if alphabetical
		if (isAlpha((int)c)) {
			str[k] = c;
			k++;
		}
	}

	// Close uncompressed file
	fclose(fp);

	// Initialize counter variables
	char a, b, c; 
	char dd[10];
	int count = 1;
	int j = 0;
	int l;

	// Compress string
	for (i = 1; i <= chunk; i++) {
		a = str[i-1];
		b = str[i];
		
		if (a == b) {
			count++;
		} else {
			if (count == 1) {
				str[j] = a;
				j++;
			} else if (count == 2) {
				str[j] = a;
				str[j+1] = a;
				j = j + 2;
			} else if (count > 2 && count < 10) {
				c = count + '0';
				str[j] = c;
				str[j+1] = a;
				j = j + 2;
			} else if (count > 9){
				sprintf(dd, "%d", count);
				for (l = 0; l < strlen(dd); l++) {
					str[j] = dd[l];
					j++;
				}
				str[j] = a;
				j = j+l-1;
			} else {
				printf("ERROR\n");
				exit(1);
			}
			count = 1;
		}
	}

	// Add null-terminating zeros to string
	while (j < chunk) {
		str[j] = '\0';
		j++;
	}

	// Open compressed file
	FILE* fp_out;
	fp_out = fopen(out_filename, "w+");

	// Verify that file was opened
	if (fp_out == NULL) {
		printf("ERROR: Proccess %d could not open file: %s\n", getpid(), out_filename);
		return;
	}

	// Write string to compressed file
	fprintf(fp_out, "%s", str);

	// Close compressed file
	fclose(fp_out);

	return;
}

/* MAIN
 *
 * @param1: "./compressR_worker_LOLS"
 * @param2: "filename.txt"
 * @param3: "out_filename.txt"
 * @param4: "chunk_size"
 * @param5: "start_pos"

 * @output: "./file_txt_LOLSx.txt"
 */
int main(int argc, char const *argv[]) {

	// Allocate space for and initialize input filename
	char * filename = (char *)calloc(strlen(argv[1]), sizeof(char)); 
	strcpy(filename, argv[1]);

	// Allocate space for and initalize output filename
	char * out_filename = (char *)calloc(strlen(argv[2]), sizeof(char));
	strcpy(out_filename, argv[2]);

	// Initialize chunk and size
	int chunk = atoi(argv[3]);
	int start = atoi(argv[4]);

	// Allocate space for compressed string
	char * str = (char *)malloc(sizeof(char)*(chunk+1));

	// Compress string
	compress(filename, out_filename, str, chunk, start);

	return 0;
}