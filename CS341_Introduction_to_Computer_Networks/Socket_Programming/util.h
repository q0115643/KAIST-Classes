#include <sys/types.h>
#include <stdint.h>

#include <ctype.h>
#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 4
#endif

#define MAXBUF 10000000

void close_socket(int sd);
void cpy_length(void *mem, ssize_t len);
unsigned short calculateChecksum(void *mem, size_t message_len);
ssize_t write_packet(int sd, void *message, size_t message_len);
ssize_t read_packet(int sd, void *answer);
uint64_t read_length(void *mem);
void hexdump(void *mem, unsigned int len);
