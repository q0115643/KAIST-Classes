/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cache.h"

#define FALSE 0
#define TRUE  1

/* Basic Information */
#define MEM_TEXT_START	0x00400000
#define MEM_TEXT_SIZE	0x00100000
#define MEM_DATA_START	0x10000000
#define MEM_DATA_SIZE	0x00100000
#define MIPS_REGS	32
#define BYTES_PER_WORD	4
#define PIPE_STAGE	5

#define IF_STAGE 	0
#define ID_STAGE	1
#define EX_STAGE	2
#define MEM_STAGE	3
#define WB_STAGE	4

/* You may add pipeline registers that you require */

// short 2 byte, unsigned char 1 byte
typedef struct inst_s {
	short opcode;

	/*R-type*/
	short func_code;

	union {
		/* R-type or I-type: */
		struct {
			unsigned char rs;
			unsigned char rt;

			union {
				short imm;

				struct {
					unsigned char rd;
					unsigned char shamt;
				} r;
			} r_i;
		} r_i;
		/* J-type: */
		uint32_t target;
	} r_t;

	uint32_t value;

	//int32 encoding;
	//imm_expr *expr;
	//char *source_line;
} instruction;

typedef struct CPU_State_Struct {
	uint32_t PC;			/* program counter for the IF stage*/
	uint32_t REGS[MIPS_REGS];	/* register file */
	uint32_t REGS_LOCK[MIPS_REGS];	/* register lock to support stalls
					   Lock registers when data is not ready*/

	uint32_t PIPE[PIPE_STAGE];	/* PC being executed at each stage*/
	uint32_t PIPE_STALL[PIPE_STAGE];

	//IF_ID_latch
	instruction* IF_ID_INST;
	uint32_t IF_ID_NPC;
	char IF_ID_COUNT;

	//ID_EX_latch
	uint32_t ID_EX_NPC;
	uint32_t ID_EX_REG1; //rs
	uint32_t ID_EX_REG2; //rt
	short ID_EX_IMM; //imm
	uint32_t ID_EX_DEST; //rd
	char ID_EX_ALUcontrol; //for ALU
	char ID_EX_ALUSrc; //I type
	char ID_EX_SHAMT;
	char ID_EX_MEM_WRITE;
	char ID_EX_BRANCH;
	char ID_EX_REG_WRITE;
	char ID_EX_MEM_READ;
	char ID_EX_FORWARDA;
	char ID_EX_FORWARDB;
	uint32_t ID_EX_FORARDV1;
	uint32_t ID_EX_FORARDV2;
	unsigned char ID_EX_RS;
	unsigned char ID_EX_RT;
	char ID_EX_COUNT;
	uint32_t ID_EX_JUMP_PC;

	//EX_MEM_latch
	uint32_t EX_MEM_NPC;
	uint32_t EX_MEM_ALU_OUT;
	uint32_t EX_MEM_W_VALUE;
	uint32_t EX_MEM_BR_TARGET;
	char EX_MEM_BR_TAKE;
	uint32_t EX_MEM_DEST;
	char EX_MEM_REG_WRITE;
	char EX_MEM_MEM_WRITE;
	char EX_MEM_MEM_READ;
	char EX_MEM_COUNT;
	uint32_t EX_MEM_JUMP_PC;

	//MEM_WB_latch
	uint32_t MEM_WB_NPC;
	uint32_t MEM_WB_ALU_OUT;
	uint32_t MEM_WB_MEM_OUT;
	char MEM_WB_BR_TAKE;
	char MEM_WB_REG_WRITE;
	char MEM_WB_MEM_TO_REG;	// memory to register
	uint32_t MEM_WB_DEST;
	uint32_t MEM_WB_BR_TARGET;
	char MEM_WB_MEM_WRITE;
	char MEM_WB_COUNT;
	uint32_t MEM_WB_W_VALUE;

	//MEM_STATE
	u_int32_t MEM_ADDR_TAG;
	unsigned char MEM_ADDR_INDEX;
	unsigned char MEM_ADDR_BLOCK_OFFSET;
	unsigned char MEM_ADDR_BYTE_OFFSET;

	int STALL_FOR_DCACHE;
	uint32_t MEM_STALL_NPC;
	uint32_t MEM_STALL_PC;
	uint32_t MEM_STALL_ALU_OUT;
	uint32_t MEM_STALL_W_VALUE;
	unsigned char MEM_STALL_DEST;
	char LOAD_USE;

	//Forwarding
	unsigned char EX_MEM_FORWARD_REG;
	unsigned char MEM_WB_FORWARD_REG;
	uint32_t EX_MEM_FORWARD_VALUE;
	uint32_t MEM_WB_FORWARD_VALUE;

	//To choose right PC
	uint32_t IF_PC;
	uint32_t BRANCH_PC;

	uint32_t NUM_CLEAR;
} CPU_State;

typedef struct {
    uint32_t start, size;
    uint8_t *mem;
} mem_region_t;

/* For PC * Registers */
extern CPU_State CURRENT_STATE;

/* For Instructions */
extern instruction *INST_INFO;
extern int NUM_INST;

/* For Memory Regions */
extern mem_region_t MEM_REGIONS[2];

/* For Execution */
extern int RUN_BIT;	/* run bit */
extern int FETCH_BIT;	/* instruction fetch bit */
extern int INSTRUCTION_COUNT;

extern int BR_BIT;	/* Branch predictor enabled */
extern int FORWARDING_BIT;
extern uint64_t MAX_INSTRUCTION_NUM;
extern uint64_t CYCLE_COUNT;

/* Functions */
char**		str_split(char *a_str, const char a_delim);
int		fromBinary(const char *s);
uint32_t	mem_read_32(uint32_t address);
void		mem_read_block(uint32_t address, uint32_t* block);
void		mem_write_32(uint32_t address, uint32_t value);
void 		mem_write_block(uint32_t address, uint32_t* block);
void		cycle();
void		run();
void		go();
void		mdump(int start, int stop);
void		rdump();
void		pdump();
void            cdump(int capacity, int assoc, int blocksize);
void            xdump(int set, int way, int blocksize, uint32_t*** cache);
void		init_memory();
void		init_inst_info();

/* YOU IMPLEMENTED THIS FUNCTION in the run.c file */
void	process_instruction();

#endif
