#include <stdio.h>
#include <sys/time.h>

#include "parser.h"
#include "file.h"
#include "sha256.h"



FileSent *open_file(Input *input) {
	FileSent *file_sent;
	file_sent = malloc(sizeof(FileSent));
	if (file_sent == NULL) {
		perror("impossible to open file");
		exit(EXIT_FAILURE);
	}

	file_sent->filename = input->file;
	file_sent->file = fopen(file_sent->filename, "r");
	if (file_sent->file == NULL) {
		perror("impossible to open file");
		exit(EXIT_FAILURE);
	}

	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
	file_sent->xor_key = rand() % 256;

	SHA256_CTX ctx;
	BYTE buffer[1024];
	size_t bytesRead;

	sha256_init(&ctx);

	while ((bytesRead = fread(buffer, 1, sizeof(buffer), file_sent->file)) !=0) {
		sha256_update(&ctx, buffer, bytesRead);
	}

	char hash[SHA256_BLOCK_SIZE];
	sha256_final(&ctx, (BYTE *)hash);

	for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
		file_sent->xored_filehash[i] = hash[i] ^ file_sent->xor_key; 
	}

	fseek(file_sent->file, 0, SEEK_SET);

	file_sent->xored_filename = malloc(sizeof(unsigned char) * strlen(file_sent->filename));
	if (file_sent->xored_filename == NULL) {
		perror("impossible to xor filename");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < strlen(file_sent->filename); i++) {
		file_sent->xored_filename[i] = file_sent->filename[i] ^ file_sent->xor_key;
	}

	return file_sent;
}


void close_file(FileSent *file_sent) {
	if (file_sent == NULL) {
		return;
	}

	fclose(file_sent->file);
	free(file_sent->xored_filename);
	free(file_sent);
}
