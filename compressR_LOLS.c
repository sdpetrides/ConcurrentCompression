// Process Manager/Parent
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "compressR_LOLS.h"

long int UNCOMP_LEN;
long int PARTS;

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
		printf("$ ./compressR_LOLS ./file.txt <int>\n");
		return 0;
	}

	// Get command line arguments
	char * filename = (char *)malloc(sizeof(char)*strlen(argv[1]));
	filename = strcpy(filename, argv[1]);
	PARTS = atoi(argv[2]);
	
	// Open uncompressed file
	FILE* fp;
	fp = fopen(argv[1], "r");	

	// Verify that file was opened
	if (fp == NULL) {
		printf("The filename could not be found\n");
		exit(1);
	}

	// Get length of uncompressed string
	char buffer[255];
	fscanf(fp, "%s", buffer);
	UNCOMP_LEN = strlen(buffer);

	// Initialize processes
	pid_t id[PARTS];
	int child_status;

	// Fork and exec() all processes
	int i;
	for (i = 0; i < PARTS; i++) {

		// Fork()
		id[i] = fork();
		
		// Exec() for child processes
		switch (id[i]) {
			case -1:
				perror("fork");
				exit(1);
			case 0:
				printf("Child:  Running process %d from parent process %d\n", getpid(), getppid());
				sleep(2); // exec() compressR_worder_LOLS.c
				exit(0);
		}
	}

	// Wait on all processes
	for (i = 0; i < PARTS; i++) {
		waitpid(id[i], &child_status, 0);
		printf("Parent: Child process %d exited with status process %d\n", id[i], child_status);
	}

	// Close uncompressed file
	fclose(fp);
	
	return 0;
}