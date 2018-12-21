#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void close_socket(int sd)
{
	close(sd);
#ifdef DEBUG
	printf("\nsocket closed\n");
#endif
}
void cpy_length(void *mem, ssize_t len)
{
	void *len_addr;
	len_addr = (void *)&len;
	memcpy(mem+0, len_addr+3, 1);
	memcpy(mem+1, len_addr+2, 1);
	memcpy(mem+2, len_addr+1, 1);
	memcpy(mem+3, len_addr+0, 1);
}
unsigned short calculateChecksum(void *mem, size_t message_len)
{
	unsigned long long sum = 0;
	void *addr = mem;
	void *addr_limit = mem + message_len;
	while(addr < addr_limit - 1)
	{
		unsigned short word16 = *(unsigned short *)addr;
		sum += word16;
		addr += 2;
	}
	if (message_len & 1)
	{
		unsigned short word16 = *(unsigned char *)addr;
		sum += word16;
	}
	while(sum >> 16)
	{
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return (unsigned short)~sum;
}

ssize_t write_packet(int sd, void *message, size_t message_len)
{
	void* write_ptr = message;
	ssize_t written_len = 0;
	while (written_len < (ssize_t)message_len)
	{
		ssize_t temp_len = write(sd, write_ptr, message_len - written_len);
		if (temp_len == -1)
		{
			printf("write_packet: write error");
			exit(-1);
		}
		else if (temp_len == 0)
		{
			printf("write_packet: lost connection");
			exit(-1);
		}
		else
		{
			write_ptr += temp_len;
			written_len += temp_len;
#ifdef DEBUG
			printf("temp_len: %d\n", (int)temp_len);
			printf("written_len: %d\n", (int)written_len);
#endif
		}
	}
	return written_len;
}

ssize_t read_packet(int sd, void *answer)
{
	void* read_ptr = answer;
	ssize_t read_len = 0;
	while (read_len < 8)
	{
		ssize_t temp_len = read(sd, read_ptr, MAXBUF);
		if (temp_len == -1)
		{
			printf("read_packet: read error 1\n");
			exit(-1);
		}
		else if (temp_len == 0)
		{
			return 0;
		}
		else
		{
			read_len += temp_len;
			read_ptr += temp_len;
#ifdef DEBUG
			printf("\ntemp_len: %d\n", (int)temp_len);
			printf("read_len: %d\n", (int)read_len);
#endif
		}
	}
	size_t len = (size_t)read_length(answer+4);
	while (read_len < len)
	{
		ssize_t temp_len = read(sd, read_ptr, len-8);
		if (temp_len == -1)
		{
			printf("read_packet: read error 2\n");
			exit(-1);
		}
		else if (temp_len == 0)
		{
			return 0;
		}
		else
		{
			read_len += temp_len;
			read_ptr += temp_len;
		}
	}
	return read_len;
}

uint64_t read_length(void *mem)
{
	uint64_t len = 0;
	void *len_addr;
	len_addr = (void *)&len;
	memcpy(len_addr+0, mem+3, 1);
	memcpy(len_addr+1, mem+2, 1);
	memcpy(len_addr+2, mem+1, 1);
	memcpy(len_addr+3, mem+0, 1);
	return len;
}


void hexdump(void *mem, unsigned int len) {
	unsigned int i, j;
	for (i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++) {
		/* print offset */
		if (i % HEXDUMP_COLS == 0) {
			printf("0x%06x: ", i);
		}
		/* print hex data */
		if (i < len) {
			printf("%02x ", 0xFF & ((char *) mem)[i]);
		} else /* end of block, just aligning for ASCII dump */
		{
			printf("   ");
		}
		/* print ASCII dump */
		if (i % HEXDUMP_COLS == (HEXDUMP_COLS - 1)) {
			for (j = i - (HEXDUMP_COLS - 1); j <= i; j++) {
				if (j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				} else if (isprint(((char *) mem)[j])) /* printable char */
				{
					putchar(0xFF & ((char *) mem)[j]);
				} else /* other char */
				{
					putchar('.');
				}
			}
			putchar('\n');
		}
	}
}