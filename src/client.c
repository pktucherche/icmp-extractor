#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "checksum.h"
#include "icmp.h"
#include "file.h"
#include "sha256.h"



void send_ping(int sockfd, struct sockaddr_in *addr, const char *data, HeaderPayload *header_payload, size_t data_len) {
	struct icmp_hdr *icmp_hdr;
	char packet[PACKET_SIZE];
	if (data_len > DATA_LEN)
		return;

	icmp_hdr = (struct icmp_hdr *) packet;

	icmp_hdr->type = ICMP_ECHO_REQUEST;
	icmp_hdr->code = 0;
	icmp_hdr->id = 0;
	icmp_hdr->sequence = 0;
	icmp_hdr->checksum = 0;

	memcpy(packet + ICMP_HEADER_LEN, header_payload, PAYLOAD_LEN);
	memcpy(packet + ICMP_HEADER_LEN + PAYLOAD_LEN, data, data_len);
	if (data_len < DATA_LEN) {
		memset(packet + ICMP_HEADER_LEN + PAYLOAD_LEN + data_len, '\0', DATA_LEN - data_len);
	}

	icmp_hdr->checksum = checksum(packet, PACKET_SIZE);

	if (sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)addr, sizeof(*addr)) <= 0) {
		perror("impossible to send echo");
		exit(EXIT_FAILURE);
	}
}


void receive_ping(int sockfd, struct sockaddr_in *addr, HeaderPayload *header_payload_sent) {
	char buffer[65536];
	socklen_t addr_len = sizeof(*addr);
	struct iphdr *ip_hdr;
	struct icmp_hdr *icmp_hdr;
	struct HeaderPayload *header_payload;
	//char *data = NULL;
	struct timeval tv;
	fd_set readfds;
	int retval;

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);

	while(1) {
		retval = select(sockfd + 1, &readfds, NULL, NULL, &tv);
		if (retval == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		} else if (retval == 0) {
			perror("Timeout, no ICMP echo reply");
			exit(EXIT_FAILURE);
		} else {
			if (recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)addr, &addr_len) <= 0) {
				perror("impossible to revfrom");
				exit(EXIT_FAILURE);
			}

			ip_hdr = (struct iphdr *)buffer;
			icmp_hdr = (struct icmp_hdr *)(buffer + (ip_hdr->ihl << 2));
			header_payload = (struct HeaderPayload *)(buffer + (ip_hdr->ihl << 2) + ICMP_HEADER_LEN);
			//data = (char *)(buffer + (ip_hdr->ihl << 2) + ICMP_HEADER_LEN + PAYLOAD_LEN);

			if (icmp_hdr->type == ICMP_ECHO_REPLY && memcmp(header_payload, header_payload_sent, PAYLOAD_LEN) == 0) {
				printf("Received ICMP echo reply\n");
				return;
			}
		}

		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
	}	
}


void client(Input *input) {
	const char *dest_addr = input->ip;
	int sockfd;
	struct sockaddr_in addr;
	struct hostent *host;

	if ((host = gethostbyname(dest_addr)) == NULL) {
		perror("impossible to gethostbyname");
		exit(EXIT_FAILURE);
	}

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		perror("impossible to create socket");
		exit(EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);

	send_file(sockfd, &addr, input);
}


void send_file(int sockfd, struct sockaddr_in *addr, Input *input) {
	FileSent *file_sent = open_file(input);

	HeaderPayload header_payload;
	header_payload.cid = CLIENT_ID;
	header_payload.num = 0;
	header_payload.total = 0;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
	header_payload.tid = rand();

	printf("cid : %x\n", header_payload.cid);
	printf("tid : %x\n", header_payload.tid);
	printf("xor_key : %x\n", file_sent->xor_key);
	printf("\n");

	char xor_key[2];
	xor_key[0] = file_sent->xor_key;
	xor_key[1] = '\0';

	send_ping(sockfd, addr, (char *)xor_key, &header_payload, strlen(xor_key));
	printf("xor_key send with ICMP echo request to %s\n", inet_ntoa(addr->sin_addr));
	receive_ping(sockfd, addr, &header_payload);

	header_payload.num = 1;
	send_ping(sockfd, addr, (char *)file_sent->xored_filename, &header_payload, strlen((char *)file_sent->xored_filename));
	printf("xor_filename send with ICMP echo request to %s\n", inet_ntoa(addr->sin_addr));
	receive_ping(sockfd, addr, &header_payload);

	header_payload.num = 2;
	send_ping(sockfd, addr, (char *)file_sent->xored_filehash, &header_payload, SHA256_BLOCK_SIZE);
	printf("xor_filehash send with ICMP echo request to %s\n", inet_ntoa(addr->sin_addr));
	receive_ping(sockfd, addr, &header_payload);
	printf("\n"),

	close_file(file_sent);
}
