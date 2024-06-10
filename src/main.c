#include <stdio.h>

#include "parser.h"
#include "client.h"



int main(int argc, char *argv[]) {
	Input *input = parser(argc, argv);
	if (input == NULL) {
		printf("BAD ARG!\n");
		return 1;
	}

	if (input->network == SERVER) {
		printf("starting server...\n");
	} else if (input->network == CLIENT) {
		printf("starting client...\n");
		printf("ip : %s\n", input->ip);
		printf("file : %s\n", input->file);

		client(input);
	}

	free(input);

	return 0;
}
