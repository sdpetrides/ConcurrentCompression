// Threads
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "compressT_LOLS.h"

// Globals
long int UNCOMP_LEN;
long int PARTS;
int CHUNK[255];
int START[255];

/* INIT_CHUNK 
 *
 * Uses globals UNCOMP_LEN and PARTS to set the chunk size
 * and start index for each thread.
 */
void init_chunk() {

	// Initialize minimum chunk size and remainder
	int min_size = (int)(UNCOMP_LEN / PARTS);
	int rem = (int)(UNCOMP_LEN % PARTS);

	// Set minimum chunk size
	int i;
	for (i = 0; i < PARTS; i++) {
		CHUNK[i] = min_size;	
	}

	// Add one to all chunks with remainder
	for (i = 0; i < rem; i++) {
		CHUNK[i]++;	
	}

	// Use start of i and size of i to set start of i+1
	START[0] = 0;
	for (i = 0; i < PARTS; i++) {
		START[i+1] = CHUNK[i] + START[i];	
	}
}

/* COMPRESS
 * 
 * @param1: uncompressed filename
 * @param2: allocated memory for compressed string
 *
 * Compression thread.
 */
void compress(Thread_data * td) {

	// Open uncompressed file
	FILE* fp;
	fp = fopen(td->filename, "r");

	// Verify that file was opened
	if (fp == NULL) {
		printf("ERROR: Thread %ld could not open file: %s\n", td->thread_id, td->filename);
		return;
	} else {
		//printf("Thread %ld has opened file: %s\n", td->thread_id, td->filename);
	}

	// Extract buffer
	char buffer[255];
	fscanf(fp, "%s", buffer);

	// Close uncompressed file
	fclose(fp);

	// COMPRESS STRING HERE
	int i;
	for (i = 0; i < CHUNK[td->thread_id]; i++) {
		td->str[i] = buffer[i+START[td->thread_id]];
	}

	// Open compressed file
	FILE* fp_out;
	fp_out = fopen(td->out_filename, "w+");

	if (fp_out == NULL) {
		printf("ERROR: Thread %ld could not open file: %s\n", td->thread_id, td->out_filename);
		return;
	}

	// Write string to compressed file
	fprintf(fp_out, "%s", td->str);

	// Close compressed file
	fclose(fp_out);

	return;
}

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
		fprintf(stderr,"Arguments given are not in valid form. Use following order:\n");
		printf("$ ./compressT_LOLS ./file.txt <int>\n");
		return 0;
	}

	// Get command line arguments
	char * filename = (char *)calloc(strlen(argv[1]), sizeof(char));
	strcpy(filename, argv[1]);
	PARTS = atoi(argv[2]);

	// Open uncompressed file
	FILE* fp;
	fp = fopen(filename, "r");	

	// Verify that file was opened
	if (fp == NULL) {
		printf("The filename could not be found\n");
		exit(1);
	}

	// Get length of uncompressed string
	char buffer[255];
	fscanf(fp, "%s", buffer);
	UNCOMP_LEN = strlen(buffer);

	// Close uncompressed file
	fclose(fp);

	// Initialize chunk sizes and start sites
	init_chunk();

	// Initialize threads, strings, and thread data
	pthread_t threads[PARTS];
	char * compressed_strings[PARTS];
	char * out_filename[PARTS];
	Thread_data * td[PARTS];

	// Create ouput filenames
	int k;
	for (k = 0; k < PARTS; k++) {
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
		char suffix[strlen(filename)+15];
		sprintf(suffix, "_LOLS%d.txt", k);
		strcat(out_filename[k], suffix);
		printf("%s\n", out_filename[k]);
	}

	// Open threads
	int flag = 0;
	long int i;
	for (i = 0; i < PARTS; i++) {

		// Allocate string for compression and output filename
		compressed_strings[i] = (char *)calloc((CHUNK[0]+1), sizeof(char));

		// Allocate and initialize thread data
		td[i] = (Thread_data *)calloc(1, sizeof(Thread_data));
		td[i]->thread_id = i;
		td[i]->filename = filename;
		td[i]->out_filename = out_filename[i];
		td[i]->str = compressed_strings[i];
		
		// Create thread
		flag = pthread_create(&threads[i], NULL, (void *)compress, (void *)td[i]);

		// Erroring checking
		if (flag) {
			fprintf(stderr, "ERROR: pthread_create() exited with status %d\n", flag);
		} else {
			//printf("Thread %ld created\n", i);
		}
	}

	// Free compressed strings, thread data, and close threads
	flag = 0;
	for (i = 0; i < PARTS; i++) {

		// Join thread
		flag = pthread_join(threads[i], NULL);

		// Free compressed string and thread data
		free(compressed_strings[i]);
		free(out_filename[i]);
		free(td[i]);

		// Erroring checking
		if (flag) {
			fprintf(stderr, "ERROR: pthread_join() exited with status %d\n", flag);
		} else {
			//printf("Thread %ld joined\n", i);
		}
	}

	// Free filename string
	free(filename);

	return 0;
}