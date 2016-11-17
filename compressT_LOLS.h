#ifndef COMPRESST_LOLS_H
#define COMPRESST_LOLS_H

// Structs
typedef struct thread_data {
	long int thread_id;
	char * filename;
	char * out_filename;
	char * str;
} Thread_data;

// Functions
void init_chunk();
void compress(Thread_data * td);

#endif