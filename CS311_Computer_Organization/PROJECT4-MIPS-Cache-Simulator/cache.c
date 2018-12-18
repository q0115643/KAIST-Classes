#include "cache.h"
#include "util.h"
#include "run.h"

/* cache.c : Implement your functions declared in cache.h */


/***************************************************************/
/*                                                             */
/* Procedure: setupCache                  		       */
/*                                                             */
/* Purpose: Allocates memory for your cache                    */
/*                                                             */
/***************************************************************/

void setupCache(int capacity, int num_way, int block_size)
{	// 64, 4, 8
/*	code for initializing and setting up your cache	*/
/*	You may add additional code if you need to	*/
	NUM_WAY = num_way;
	BLOCK_SIZE = block_size;
	int i,j; //counter
	int nset=0; // number of sets 2
	int _wpb=0; //words per block 2
	nset=capacity/(block_size*num_way); // 2
	_wpb = block_size/BYTES_PER_WORD; // 2

	Cache = (uint32_t  ***)malloc(nset*sizeof(uint32_t **));
	Cache_Tag = (uint32_t  **)malloc(nset*sizeof(uint32_t *));
	Cache_Usage_Order = (uint32_t  **)malloc(nset*sizeof(uint32_t *));

	for (i=0;i<nset;i++) {
		Cache[i] = (uint32_t ** )malloc(num_way*sizeof(uint32_t*));
		Cache_Tag[i] = (uint32_t * )malloc(num_way*sizeof(uint32_t));
		Cache_Usage_Order[i] = (uint32_t * )malloc(num_way*sizeof(uint32_t));
		int k;
		for (k=0;k<num_way;k++)
		{
			Cache_Usage_Order[i][k] = 0;
			Cache_Tag[i][k] = 0;
		}
	}
	for (i=0; i<nset; i++){	
		for (j=0; j<num_way; j++){ // 2set * 4way * 2word
			Cache[i][j]=(uint32_t*)malloc(sizeof(uint32_t)*(_wpb));
			int q;
			for (q=0;q<_wpb;q++){
				Cache[i][j][q]=0;
			}
		}
	}

}


/***************************************************************/
/*                                                             */
/* Procedure: setCacheMissPenalty                  	       */
/*                                                             */
/* Purpose: Sets how many cycles your pipline will stall       */
/*                                                             */
/***************************************************************/

void setCacheMissPenalty(int penalty_cycles)
{
/*	code for setting up miss penalty			*/
/*	You may add additional code if you need to	*/	
	miss_penalty = penalty_cycles;

}

/* Please declare and implement additional functions for your cache */
void getDRAM(){
	int picked = 0;
	int pickedWay = 0;
	int way;
	for (way=0;way<NUM_WAY;way++) {
		if (Cache_Usage_Order[SET][way]==0 && picked==0) {
			mem_read_block(TARGET_ADDRESS, Cache[SET][way]);
			picked = 1;
			pickedWay = way;
			Cache_Usage_Order[SET][way] = 1;
			Cache_Tag[SET][way] = CURRENT_STATE.MEM_ADDR_TAG;
		}
	}
	if(picked)
	{
		for (way=0;way<NUM_WAY;way++)
		{
			if (Cache_Usage_Order[SET][way]>0)
			{
				if (way!=pickedWay)
				{
					Cache_Usage_Order[SET][way]++;
				}
			}
		}
	}
	if(!picked)
	{
		for (way=0;way<NUM_WAY;way++)
		{
			if (Cache_Usage_Order[SET][way]==NUM_WAY && picked==0)
			{
				uint32_t addr = ((Cache_Tag[SET][way]<<4)&0xFFFFFFF0)|(SET<<3);
				mem_write_block(addr, Cache[SET][way]);
				mem_read_block(TARGET_ADDRESS, Cache[SET][way]);
				Cache_Usage_Order[SET][way]=1;
				Cache_Tag[SET][way] = CURRENT_STATE.MEM_ADDR_TAG;
				pickedWay = way;
				picked = 1;
			}
		}
		for (way=0;way<NUM_WAY;way++)
		{
			if (way!=pickedWay)
			{
				Cache_Usage_Order[SET][way]++;
			}
		}
	}
}

void categorizeAddress()
{
	TARGET_ADDRESS = CURRENT_STATE.MEM_WB_ALU_OUT;
	CURRENT_STATE.MEM_ADDR_TAG = (TARGET_ADDRESS >> 4) & 0x0FFFFFFF;
	CURRENT_STATE.MEM_ADDR_INDEX = (unsigned char)((TARGET_ADDRESS >> 3) & 1);
	CURRENT_STATE.MEM_ADDR_BLOCK_OFFSET = (unsigned char)((TARGET_ADDRESS >> 2) & 1);
	CURRENT_STATE.MEM_ADDR_BYTE_OFFSET = (unsigned char)(TARGET_ADDRESS & 3);
}

void checkHit()
{
	CACHE_HIT = 0;
	SET = CURRENT_STATE.MEM_ADDR_INDEX;
	WORD = CURRENT_STATE.MEM_ADDR_BLOCK_OFFSET;
	int way;
	for (way=0;way<NUM_WAY;way++)
	{
		if (Cache_Tag[SET][way]==CURRENT_STATE.MEM_ADDR_TAG)
		{
			WAY = way;
			CACHE_HIT = 1;
			int i;
			for (i=0;i<NUM_WAY;i++)
			{
				if (Cache_Usage_Order[SET][i]<Cache_Usage_Order[SET][way] && Cache_Usage_Order[SET][i]!=0)
				{
					Cache_Usage_Order[SET][i]++;
				}
			}
			Cache_Usage_Order[SET][way]=1;
		}
		else if(CACHE_HIT==0)
		{
			CACHE_HIT = 0;
		}
	}
}

void WRITE(u_int32_t W_VALUE)
{
	if(CURRENT_STATE.STALL_FOR_DCACHE)
	{
		getDRAM();
	}
	checkHit();
	if(CACHE_HIT)
	{
		Cache[SET][WAY][WORD] = W_VALUE;
		SET = 0;
		WAY = 0;
		WORD = 0;
		CACHE_PANELTY_COUNT = 0;
	}
	else
	{
		CACHE_PANELTY_COUNT = miss_penalty;
	}
}

void READ()
{
	if(CURRENT_STATE.STALL_FOR_DCACHE)
	{
		getDRAM();
	}
	checkHit();
	if(CACHE_HIT)
	{
		CURRENT_STATE.MEM_WB_MEM_OUT = Cache[SET][WAY][WORD];
		SET = 0;
		WAY = 0;
		WORD = 0;
		CACHE_PANELTY_COUNT = 0;
	}
	else
	{
		CACHE_PANELTY_COUNT = miss_penalty;
	}
}











