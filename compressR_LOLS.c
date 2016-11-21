#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "compressR_LOLS.h"

long int UNCOMP_LEN;
long int PARTS;
int * CHUNK;
int * START;

/* INIT_OUT_NAME 
 *
 * Uses filename and PARTS to set the output filename
 * for each thread.
 */
void init_out_name(char * out_filename[], char * filename) {

	int k;
	for (k = 0; k < PARTS; k++) {

		// Copy filename into newly allocated memory
		out_filename[k] = (char *)calloc(strlen(filename)+20, sizeof(char));
		strcpy(out_filename[k], filename);

		// Replace '.' with '_'
		int j;
		for (j = 1; j < strlen(filename); j++) {
			if (out_filename[k][j] == '.') {
				out_filename[k][j] = '_';
			}
		}

		// Concatonate _LOLSX.txt to filename
		if (PARTS == 1) {
			strcat(out_filename[k], "_LOLS.txt");
		} else {
			char suffix[strlen(filename)+15];
			sprintf(suffix, "_LOLS%d.txt", k);
			strcat(out_filename[k], suffix);
		}	
	}
}

/* INIT_CHUNK 
 *
 * Uses globals UNCOMP_LEN and PARTS to set the chunk size
 * and start index for each thread.
 */
void init_chunk() {

	// Initialize and allocate memory for parts and start values	
	CHUNK = (int *)malloc(sizeof(int)*PARTS);
	START = (int *)malloc(sizeof(int)*PARTS);

	// Initialize minimum chunk size and remainder
	int min_size = (int)(UNCOMP_LEN / PARTS);
	int rem = (int)(UNCOMP_LEN % PARTS);

	if (!min_size) {
		printf("ERROR: Incorrect division of characters and parts\n");
		exit(1);
	}

	// Set minimum chunk size
	int i;
	for (i = 0; i < PARTS; i++) {
		CHUNK[i] = min_size;	
	}

	// Add remainder to first chunk
	CHUNK[0]+=rem;

	// Use start of i and size of i to set start of i+1
	START[0] = 0;
	for (i = 0; i < PARTS; i++) {
		START[i+1] = CHUNK[i] + START[i];	
	}
}

/* MAIN
 *
 * @param1: "./file.txt"
 * @param2: <int>
 * 
 * @output: "./file_txt_LOLSx.txt" 
 */
int main(int argc, char const *argv[]) {

	// Check for correct parameters
	if (argc != 3) {
		fprintf(stderr,"Arguments given are not in valid form. Use following order:\n");
		printf("$ ./compressR_LOLS ./file.txt <int>\n");
		return 0;
	}

	// Get command line arguments
	char * filename = (char *)calloc(strlen(argv[1]), sizeof(char));
	filename = strcpy(filename, argv[1]);
	PARTS = atoi(argv[2]);

	// Verify that PARTS value is in the correct range
	if (PARTS < 1) {
		printf("ERROR: The number of parts must be at least one\n");
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

	// Get length of uncompressed string
	UNCOMP_LEN = 0;
	char c;
	while ((c = fgetc(fp))) {
    	if (c == EOF) {
    		break;
    	}
		UNCOMP_LEN+=1;
	}

	// Close uncompressed file
	fclose(fp);

	// Initialize chunk and start values
	init_chunk();

	// Initialize output filenames
	char * out_filename[PARTS];
	init_out_name(out_filename, filename);

	// Initialize processes
	pid_t id[PARTS];
	char * chunk_str[PARTS];
	char * start_str[PARTS];
	int child_status;

	// Initialize arguments array
	char ** arguments[PARTS];

	// Fork and exec() all processes
	int i;
	int flag = 0;
	for (i = 0; i < PARTS; i++) {

		// Allocate space for chunk and start strings
		chunk_str[i] = (char *)calloc(10, sizeof(char));
		start_str[i] = (char *)calloc(10, sizeof(char));

		// Format chunk and start strings
		sprintf(chunk_str[i], "%d", CHUNK[i]);
		sprintf(start_str[i], "%d", START[i]);

		// Allocate space for the arguments
		arguments[i] = (char **)malloc(sizeof(char *)*6);

		// Initialize the arguments
		arguments[i][0] = "./compressR_worker_LOLS";
		arguments[i][1] = filename;
		arguments[i][2] = out_filename[i];
		arguments[i][3] = chunk_str[i];
		arguments[i][4] = start_str[i];
		arguments[i][5] = NULL;

		// Fork()
		id[i] = fork();
		
		// Exec() for child processes
		switch (id[i]) {
			case -1:
				perror("fork");
				exit(1);
			case 0:

				// Execute compressR_worker_LOLS
				flag = execvp("./compressR_worker_LOLS", arguments[i]);

				// Error check for exec()
				if (flag == -1) {
					printf("ERROR: exec() failed\n");
					exit(1);
				} else {
					exit(1);
				}
		}
	}

	// Wait on all processes
	for (i = 0; i < PARTS; i++) {

		// Wait for each PID
		waitpid(id[i], &child_status, 0);

		// Free argument strings
		free(arguments[i]);
		free(chunk_str[i]);
		free(start_str[i]);
		free(out_filename[i]);
	}

	// Free filename
	free(filename);

	return 0;
}