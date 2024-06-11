#ifndef CLIENT_H_
#define CLIENT_H_

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <sys/time.h>

#include "icmp.h"
#include "parser.h"

#define CLIENT_ID 1817628043

void send_ping(int sockfd, struct sockaddr_in *addr, const char *data, HeaderPayload *header_payload, size_t data_len);
void client(Input *input);
void receive_ping(int sockfd, struct sockaddr_in *addr, HeaderPayload *header_payload_sent);
void send_file(int sockfd, struct sockaddr_in* addr, Input *input);

#endif
