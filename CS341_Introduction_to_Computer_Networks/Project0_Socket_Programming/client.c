#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdint.h>
#include "util.h"

//#define DEBUG

void print_answer(unsigned char* answer, ssize_t len);

int main(int argc, char *argv[])
{
	int optind, sd;
	char* h;
	uint8_t o, s;
	int p;
	for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++) {
		switch (argv[optind][1]) {
			case 'h': h = argv[++optind]; break;	// HOST
			case 'p': p = atoi(argv[++optind]); break;	// PORT
			case 'o': o = (uint8_t) atoi(argv[++optind]); break;	// Operation (encrypt if 0, decrypt if 1)
			case 's': s = (uint8_t) atoi(argv[++optind]); break;	// Shift
			default:
				printf("Usage: ./client –h [host] –p [port] –o [operation] –s [shift]\n");
				exit(-1);
		}
	}
	if (o == 0)
	{
#ifdef DEBUG
		printf("Host: %s, Port: %d, Operation: encrypt, Shift %d\n", h, p, s);
#endif
	}
	else if (o == 1)
	{
#ifdef DEBUG
		printf("Host: %s, Port: %d, Operation: decrypt, Shift %d\n", h, p, s);
#endif
	}
	else{
		printf("Operation should be 0(encrypt) or 1(decrypt)\n");
		exit(-1);
	}
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket creation failure\n");
		exit(-1);
	}
#ifdef DEBUG
	printf("socket created\n");
#endif
	in_addr_t addr = inet_addr(h);
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(p);
	server_addr.sin_addr.s_addr = addr;
	int server_sd;
	if ((server_sd = connect(sd, (struct sockaddr*)&server_addr, sizeof(server_addr))) == -1)
	{
		printf("socket connection failure\n");
		close_socket(sd);
		exit(-1);
	}
#ifdef DEBUG
	printf("sockect connected\n");
#endif
	unsigned char* message = (unsigned char*) malloc (sizeof(unsigned char)*(MAXBUF+1));
	unsigned char* answer = (unsigned char*) malloc (sizeof(unsigned char)*(MAXBUF+1));
	memset(message, 0x00, MAXBUF+1);
	memset(answer, 0x00, MAXBUF+1);
	memcpy(message, &o, 1);
	memcpy(message+1, &s, 1);

	ssize_t len;
	while ((len = read(STDIN_FILENO, message + 8, MAXBUF - 8)) > 0)	// read each (MAXBUF-8) characters
	{
		size_t message_len = (size_t)len + 8;
#ifdef DEBUG
		printf("message length: %d\n", (unsigned int)message_len);
		printf("%x\n", (unsigned int)message_len);
#endif
		cpy_length(message+4, len+8);
		unsigned short checksum = calculateChecksum(message, message_len);
		memcpy(message+2, &checksum, 2);
		if(calculateChecksum(message, message_len))
		{
			printf("checksum error from written message\n");
			exit(-1);
		}
		ssize_t write_len = write_packet(sd, message, message_len);
		if (write_len != message_len)
		{
			printf("written message length error\n");
			exit(-1);
		}
#ifdef DEBUG
		printf("write_len: %d\n", (int)write_len);
		hexdump(message, 16);
		printf("message written\n");
#endif
		ssize_t read_len = read_packet(sd, answer);
		if (!read_len)
		{
			printf("read_packet: lost connection 1\n");
			exit(-1);
		}
#ifdef DEBUG
		printf("\n%d read_len\n", (int)read_len);
		hexdump(answer, 16);
		printf("answer read\n");
#endif
		if(calculateChecksum(answer, (size_t)read_len))
		{
			printf("checksum error from read answer\n");
			exit(-1);
		}
		print_answer(answer, len);
		memset(message+2, 0x00, MAXBUF-2);
		memset(answer, 0x00, MAXBUF);
	}
	close_socket(sd);
	return 0;
}

void print_answer(unsigned char* answer, ssize_t len)
{
	printf("%s", answer+8);
};