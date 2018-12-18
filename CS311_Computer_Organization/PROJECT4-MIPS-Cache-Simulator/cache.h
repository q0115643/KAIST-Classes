#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* cache.h : Declare functions and data necessary for your project*/

int miss_penalty; // number of cycles to stall when a cache miss occurs
uint32_t ***Cache; // data cache storing data [set][way][byte]
uint32_t **Cache_Tag; // data cache storing TAG [set][way][byte]
uint32_t **Cache_Usage_Order;

int NUM_WAY;
int BLOCK_SIZE;

void setupCache(int, int, int);
void setCacheMissPenalty(int);

void categorizeAddress();
void WRITE(u_int32_t);
void READ();
void checkHit();

u_int32_t TARGET_ADDRESS;
char CACHE_HIT;

int SET;
int WORD;
int WAY;

int CACHE_PANELTY_COUNT;
