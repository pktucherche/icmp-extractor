#ifndef FILE_H_
#define FILE_H_

#include <stdio.h>

#include "parser.h"
#include "sha256.h"

typedef struct FileSent {
	char *filename;
	FILE *file;
	unsigned char xor_key;
	unsigned char *xored_filename;
	unsigned char xored_filehash[SHA256_BLOCK_SIZE];
	char chunk[512];
	size_t nb_chunk;
	size_t size_file;
} FileSent;

FileSent *open_file(Input *input); 
void close_file(FileSent *file_sent);

#endif
