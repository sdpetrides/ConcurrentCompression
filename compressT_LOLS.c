// Threads
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "compressT_LOLS.h"

long int UNCOMP_LEN;
long int PARTS;

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
		printf("Thread %ld could not open file: %s\n", td->thread_id, td->filename);
		return;
	} else {
		printf("Thread %ld has opened file: %s\n", td->thread_id, td->filename);
	}

	char buffer[255];
	fscanf(fp, "%s", buffer);

	// Compress string
	int i;
	for (i = 0; i < td->chunk; i++) {
		td->str[i] = buffer[i+td->start];
	}

	// Close uncompressed file
	fclose(fp);

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
	char * filename = (char *)malloc(sizeof(char)*strlen(argv[1]));
	filename = strcpy(filename, argv[1]);
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

	// Initialize threads, strings, and thread data
	pthread_t threads[PARTS];
	char * compressed_strings[PARTS];
	Thread_data * td[PARTS];

	// Open threads
	int flag = 0;
	long int i;
	for (i = 0; i < PARTS; i++) {

		// Allocate string for compression
		compressed_strings[i] = (char *)malloc(sizeof(char)*10);

		// Allocate and initialize thread data
		td[i] = (Thread_data *)malloc(sizeof(Thread_data));
		td[i]->thread_id = i;
		td[i]->chunk = (UNCOMP_LEN/PARTS);
		td[i]->start = i*(UNCOMP_LEN/PARTS);
		td[i]->filename = filename;
		td[i]->str = compressed_strings[i];
		
		// Create thread
		flag = pthread_create(&threads[i], NULL, (void *)compress, (void *)td[i]);

		// Erroring checking
		if (flag) {
			fprintf(stderr, "ERROR: pthread_create() exited with status %d\n", flag);
		} else {
			printf("Thread %ld created\n", i);
		}
	}

	// Free compressed strings, thread data, and close threads
	flag = 0;
	for (i = 0; i < PARTS; i++) {

		// Free compressed string and thread data
		printf("%s\n", compressed_strings[i]);
		free(compressed_strings[i]);
		free(td[i]);

		// Join thread
		flag = pthread_join(threads[i], NULL);

		// Erroring checking
		if (flag) {
			fprintf(stderr, "ERROR: pthread_join() exited with status %d\n", flag);
		} else {
			printf("Thread %ld joined\n", i);
		}
	}

	return 0;
}