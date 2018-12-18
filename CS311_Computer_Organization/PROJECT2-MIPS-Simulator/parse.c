/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
	/** Implement this function */
	char line[33];
	strcpy(line, buffer);

	int opcode;
	int rs, rt, rd, shamt, funct;
	int imm, target;

	int inst = fromBinary(line);

	opcode = (inst >> 26) & 0x3f;

	instr.opcode = (short) opcode;
	instr.value = (uint32_t) inst;

	int address = MEM_TEXT_START + index;
	mem_write_32((uint32_t) address, instr.value);


	if(opcode==0) {
		// R format
		rs = (inst >> 21) & 0x1f;
		rt = (inst >> 16) & 0x1f;
		rd = (inst >> 11) & 0x1f;
		shamt = (inst >> 6) & 0x1f;
		funct = inst & 0x3f;

		instr.r_t.r_i.rs = (unsigned char) rs;
		instr.r_t.r_i.rt = (unsigned char) rt;
		instr.r_t.r_i.r_i.r.rd = (unsigned char) rd;
		instr.r_t.r_i.r_i.r.shamt = (unsigned char) shamt;
		instr.func_code = (short) funct;
	}
	else if(opcode==2 || opcode==3) {
		// J format
		target = inst & 0x3ffffff;

		instr.r_t.target = (uint32_t) target;
	}
	else {
		// I format
		rs = (inst >> 21) & 0x1f;
		rt = (inst >> 16) & 0x1f;
		imm = inst & 0xffff;

		instr.r_t.r_i.rs = (unsigned char) rs;
		instr.r_t.r_i.rt = (unsigned char) rt;
		instr.r_t.r_i.r_i.imm = (short) imm;
	}

    return instr;
}

void parsing_data(const char *buffer, const int index)
{
	/** Implement this function */
	char line[33];
	strcpy(line, buffer);
	int data = fromBinary(line);

	int address = MEM_DATA_START + index;
	mem_write_32((uint32_t) address, (uint32_t) data);
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
	printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
	printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	switch(INST_INFO[i].opcode)
	{
	    //Type I
	    case 0x9:		//(0x001001)ADDIU
	    case 0xc:		//(0x001100)ANDI
	    case 0xf:		//(0x001111)LUI	
	    case 0xd:		//(0x001101)ORI
	    case 0xb:		//(0x001011)SLTIU
	    case 0x23:		//(0x100011)LW	
	    case 0x2b:		//(0x101011)SW
	    case 0x4:		//(0x000100)BEQ
	    case 0x5:		//(0x000101)BNE
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
		break;

    	    //TYPE R
	    case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
		printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
		printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
		break;

    	    //TYPE J
	    case 0x2:		//(0x000010)J
	    case 0x3:		//(0x000011)JAL
		printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
		break;

	    default:
		printf("Not available instruction\n");
		assert(0);
	}
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
	printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
	printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
