#ifndef PARSER_H_
#define PARSER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef enum Network {
	SERVER,
	CLIENT,
	NOT_DEFINE
} Network;

typedef struct Input {
	Network network;
	char *file;	
	char *ip;
} Input;


Input *parser(int argc, char *argv[]);


#endif
