#include "parser.h"



Input *parser(int argc, char *argv[]) {
	if (argc <= 1) {
		return NULL;
	}

	Input *input = malloc(sizeof(Input));
	if (input == NULL) { return NULL; }
	input->network = NOT_DEFINE;
	input->file = NULL;
	input->ip = NULL;

	int i;
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--server")) {
			if (input->network != NOT_DEFINE) { 
				free(input);
				return NULL;
			}
			input->network = SERVER;
		} else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--client")) {
			if (input->network != NOT_DEFINE) { 
				free(input);
				return NULL;
			}
			input->network = CLIENT;
			i++;
			if (i == argc) {
				free(input);
				return NULL;
			}
			input->ip = argv[i];
		} else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--file")) {
			i++;
			if (i == argc) {
				free(input);
				return NULL;
			}
			input->file = argv[i];
		}
	}

	if (input->network == NOT_DEFINE) {
		free(input);
		return NULL;
	}
	if (input->network == CLIENT && (input->file == NULL || input->ip == NULL)) {
		free(input);
		return NULL;
	}
	if (input->network == SERVER && argc != 2) {
		free(input);
		return NULL;
	}

	return input;
}
