#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util.h"

//#define DEBUG

void caesarCipher(void* message, ssize_t len);
int is_alphabet(char c);

int main(int argc, char *argv[])
{
	pid_t child;
	unsigned char* message = (unsigned char*) malloc (sizeof(unsigned char)*(MAXBUF+1));
	memset(message, 0x00, MAXBUF);
	struct sockaddr_in server_addr, client_addr;
	in_addr_t addr = inet_addr("127.0.0.1");
	int listen_fd, client_fd;
	if(argc != 3)
	{
		printf("Usage: ./server –p [port]\n");
		return 1;
	}
	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("server socket creation failure\n");
		return 1;
	}
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = addr;
	server_addr.sin_port = htons(atoi(argv[2]));
	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		printf("server bind error\n");
		return 1;
	}
	if(listen(listen_fd, 5) == -1)
	{
		printf("server listen error\n");
		return 1;
	}
	while(1)
	{
		socklen_t len = sizeof(client_addr);
		client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len);
		if(client_fd == -1)
		{
			printf("server accept failed.\n");
			exit(-1);
		}
		child = fork();
		if (child == 0)
		{
			close_socket(listen_fd);
			size_t read_len;
			while((read_len = (size_t)read_packet(client_fd, message)) > 0)
			{
				if(calculateChecksum(message, read_len))
				{
					printf("checksum error from written message\n");
					printf("%d\n", (int)read_len);
					exit(-1);
				}
				memset(message+2, 0x00, 2);
				caesarCipher(message, read_len);
				unsigned short checksum = calculateChecksum(message, read_len);
				memcpy(message+2, &checksum, 2);
				ssize_t write_len = write_packet(client_fd, message, read_len);
				memset(message, 0x00, write_len);
			}
			close_socket(client_fd);
			exit(0);
		}
		else
		{
			close_socket(client_fd);
		}
	}
	close_socket(listen_fd);
	return 0;
}

void caesarCipher(void* message, ssize_t len)
{
	void* ptr = message;
	int bytes_read = 0;
	uint8_t op = *(uint8_t*)ptr;
	uint8_t shift = *(uint8_t*)(ptr+1);
	ptr += 8;
	bytes_read += 8;
	while(bytes_read < len)
	{
		char c = *(unsigned char*)ptr;
		if(is_alphabet(c))
		{
			int c_low = tolower(c);
			if(op)
			{
				c_low = ((c_low - 'a') - shift) % 26;
				if(c_low < 0)
				{
					c_low += 26;
				}
				c_low = c_low + 'a';
			}
			else
			{
				c_low = ((c_low - 'a') + shift) % 26 + 'a';
			}
			*(unsigned char*)ptr = (unsigned char)c_low;
		}
		ptr += 1;
		bytes_read += 1;
	}
}

int is_alphabet(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

