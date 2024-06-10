#ifndef ICMP_H_
#define ICMP_H_


#define ICMP_HEADER_LEN 8
#define PAYLOAD_LEN 16
#define DATA_LEN 512
#define PACKET_SIZE 536

typedef struct HeaderPayload {
	int cid;
	int tid;
	int num;
	int total;
} HeaderPayload;


#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8

typedef struct icmp_hdr {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t sequence;
} icmp_hdr;


#endif
