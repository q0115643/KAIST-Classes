#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "util.h"

#define FD_SETSIZE 128
//#define DEBUG

void caesarCipher(void* message, ssize_t len);
int is_alphabet(char c);

int main(int argc, char *argv[])
{
	unsigned char **message = malloc(FD_SETSIZE * sizeof(unsigned char *));
	for(int i = 0; i < FD_SETSIZE; i++) {
		message[i] = malloc((MAXBUF + 1) * sizeof(unsigned char));
		memset(message[i], 0x00, MAXBUF + 1);
	}
	void** read_ptrs = malloc(FD_SETSIZE * sizeof(void*));
	void** write_ptrs = malloc(FD_SETSIZE * sizeof(void*));
	unsigned int read_signal[FD_SETSIZE];
	unsigned int read_lens[FD_SETSIZE];
	unsigned int written_lens[FD_SETSIZE];
	unsigned int message_lens[FD_SETSIZE];
	memset(read_ptrs, 0x00, FD_SETSIZE);
	memset(write_ptrs, 0x00, FD_SETSIZE);
	for (int i=0; i < FD_SETSIZE; i++)
	{
		read_ptrs[i] = message[i];
		write_ptrs[i] = message[i];
	}
	memset(read_signal, 0x00, FD_SETSIZE);
	memset(read_lens, 0x00, FD_SETSIZE * 4);
	memset(written_lens, 0x00, FD_SETSIZE * 4);
	memset(message_lens, 0x00, FD_SETSIZE * 4);

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
#ifdef DEBUG
	printf("server socket created.\n");
#endif
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = addr;
	server_addr.sin_port = htons(atoi(argv[2]));
	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		printf("server bind error\n");
		return 1;
	}
#ifdef DEBUG
	printf("server bound.\n");
#endif
	if(listen(listen_fd, 5) == -1)
	{
		printf("server listen error\n");
		return 1;
	}
#ifdef DEBUG
	printf("server listening.\n");
#endif

	int maxfd = 0;
	int maxfd_read = 0;
	int fd_num;
	fd_set base_fds, read_fds;
	FD_ZERO(&base_fds);
	FD_SET(listen_fd, &base_fds);
	maxfd = listen_fd;
	while(1)
	{
		read_fds = base_fds;
		maxfd_read = maxfd;
		if ((fd_num = select(maxfd + 1, &read_fds, NULL, NULL, NULL)) == -1) {
			if (errno == EINTR)
				continue;
			break;
		}
		for (int i = 0; i <= maxfd_read; i++)
		{
			int fd = i;
			if(FD_ISSET(fd, &read_fds))
			{
				if (fd == listen_fd) {
					socklen_t len = sizeof(client_addr);
					client_fd = accept(fd, (struct sockaddr*)&client_addr, &len) == -1;
					if (client_fd == -1)
					{
						printf("server accept failed.\n");
						exit(-1);
					}
					FD_SET(client_fd, &base_fds);
					if (maxfd < client_fd)
					{
						maxfd = client_fd;
					}
				}
				else
				{
					if(!read_signal[fd])
					{
						//read
						if (read_lens[fd] < 8)
						{
							ssize_t temp_len = read(fd, read_ptrs[fd], MAXBUF);
							if (temp_len == -1)
							{
								if (errno == EINTR)
									continue;
								FD_CLR(fd, &base_fds);
								memset(message[fd], 0x00, MAXBUF + 1);
								read_lens[fd] = 0;
								read_ptrs[fd] = message[fd];
								continue;
							}
							else if (temp_len == 0)
							{
								FD_CLR(fd, &base_fds);
								memset(message[fd], 0x00, MAXBUF + 1);
								read_lens[fd] = 0;
								read_ptrs[fd] = message[fd];
								continue;
							}
							else
							{
								read_lens[fd] += temp_len;
								read_ptrs[fd] += temp_len;
							}
						}
						else
						{
							size_t temp_message_len = (size_t)read_length(message[fd]+4);
							if (read_lens[fd] < temp_message_len)
							{
								ssize_t temp_len = read(fd, read_ptrs[fd], MAXBUF);
								if (temp_len == -1)
								{
									if (errno == EINTR)
										continue;
									FD_CLR(fd, &base_fds);
									memset(message[fd], 0x00, MAXBUF + 1);
									read_lens[fd] = 0;
									read_ptrs[fd] = message[fd];
									continue;
								}
								else if (temp_len == 0)
								{
									FD_CLR(fd, &base_fds);
									memset(message[fd], 0x00, MAXBUF + 1);
									read_lens[fd] = 0;
									read_ptrs[fd] = message[fd];
									continue;
								}
								else
								{
									read_lens[fd] += temp_len;
									read_ptrs[fd] += temp_len;
								}

							}
							if (read_lens[fd] == temp_message_len)
							{
								read_lens[fd] = 0;
								read_ptrs[fd] = message[fd];
								read_signal[fd] = 1;
								message_lens[fd] = (unsigned int)temp_message_len;
							}
						}
					}
					else
					{
						//write
						if (written_lens[fd] < message_lens[fd])
						{
							ssize_t temp_len = write(fd, write_ptrs[fd], message_lens[fd] - written_lens[fd]);
							if (temp_len == -1)
							{
								if (errno == EINTR)
									continue;
								FD_CLR(fd, &base_fds);
								memset(message[fd], 0x00, MAXBUF + 1);
								written_lens[fd] = 0;
								write_ptrs[fd] = message[fd];
								continue;
							}
							else if (temp_len == 0)
							{
								FD_CLR(fd, &base_fds);
								memset(message[fd], 0x00, MAXBUF + 1);
								written_lens[fd] = 0;
								write_ptrs[fd] = message[fd];
								continue;
							}
							else
							{
								write_ptrs[fd] += temp_len;
								written_lens[fd] += temp_len;
							}
						}
						if (written_lens[fd] == message_lens[fd])
						{
							write_ptrs[fd] = message[fd];
							written_lens[fd] = 0;
							read_signal[fd] = 0;
							message_lens[fd] = 0;
						}
					}
				}
				if (--fd_num <= 0)
				{
					break;
				}
			}
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
