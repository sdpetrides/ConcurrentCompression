#ifndef COMPRESST_LOLS_H
#define COMPRESST_LOLS_H

// Structs
typedef struct thread_data {
	long int thread_id;
	int parts;
	char * filename;
	char * str;
} Thread_data;

// Functions
void compress(Thread_data * td);

#endif