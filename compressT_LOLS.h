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
int isAlpha(int c);
void init_out_name(char * out_filename[], char * filename);
void init_chunk();
void compress(Thread_data * td);

#endif