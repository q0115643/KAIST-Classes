/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) 
{ 
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
	/** Implement this function */

    // fetching instruction
    int instrIndex = (CURRENT_STATE.PC - MEM_TEXT_START) / 4;

    instruction instr = INST_INFO[instrIndex];
    CURRENT_STATE.PC += 4;
    short opcode, func_code, imm;
    unsigned char rs, rt, rd, shamt;
    uint32_t  target, value;

    opcode = instr.opcode;
    value = instr.value;

    switch(opcode)
    {
            //TYPE R
        case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
            rs = instr.r_t.r_i.rs;
            rt = instr.r_t.r_i.rt;
            rd = instr.r_t.r_i.r_i.r.rd;
            shamt = instr.r_t.r_i.r_i.r.shamt;
            func_code = instr.func_code;
            switch(func_code)
            {
                case 0x24:
                    //AND
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    break;
                case 0x21:
                    //ADDU
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    break;
                case 0x27:
                    //NOR
                    CURRENT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                    break;
                case 0x25:
                    //OR
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
                    break;
                case 0x2b:
                    //SLTU
                    if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) {
                        CURRENT_STATE.REGS[rd] = 1;
                    }
                    else {
                        CURRENT_STATE.REGS[rd] = 0;
                    }
                    break;
                case 0x00:
                    //SLL
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
                    break;
                case 0x02:
                    //SRL
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                    break;
                case 0x23:
                    //SUBU
                    CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    break;
                case 0x08:
                    //JR
                    CURRENT_STATE.PC = CURRENT_STATE.REGS[rs];
                    break;
                default:
                    printf("Not available instruction\n");
                    assert(0);
            }
            break;

            //TYPE J
        case 0x2:		//(0x000010)J
            target = instr.r_t.target;
            CURRENT_STATE.PC = ((CURRENT_STATE.PC - 4) & 0xf0000000) | (target << 2);
            break;
        case 0x3:		//(0x000011)JAL
            target = instr.r_t.target;
            CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            CURRENT_STATE.PC = ((CURRENT_STATE.PC - 4) & 0xf0000000) | (target << 2);
            break;

            //Type I
        default:
            rs = instr.r_t.r_i.rs;
            rt = instr.r_t.r_i.rt;
            imm = instr.r_t.r_i.r_i.imm;
            switch(opcode)
            {
                case 0x9:		//(0x001001)ADDIU
                    CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
                    break;
                case 0xc:		//(0x001100)ANDI
                    CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm;
                    break;
                case 0xf:		//(0x001111)LUI
                    CURRENT_STATE.REGS[rt] = (uint32_t) imm << 16;
                    break;
                case 0xd:		//(0x001101)ORI
                    CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm;
                    break;
                case 0xb:		//(0x001011)SLTIU
                    if (CURRENT_STATE.REGS[rs] < imm) {
                        CURRENT_STATE.REGS[rt] = 1;
                    }
                    else {
                        CURRENT_STATE.REGS[rt] = 0;
                    }
                    break;
                case 0x23:		//(0x100011)LW
                    CURRENT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + imm);
                    break;
                case 0x2b:		//(0x101011)SW
                    mem_write_32(CURRENT_STATE.REGS[rs] + imm, CURRENT_STATE.REGS[rt]);
                    break;
                case 0x4:		//(0x000100)BEQ
                    if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
                        CURRENT_STATE.PC += imm << 2;
                    }
                    break;
                case 0x5:		//(0x000101)BNE
                    if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
                        CURRENT_STATE.PC += imm << 2;
                    }
                    break;
                default:
                    printf("instrIndex: %d\n", instrIndex);
                    printf("Not available instruction\n");
                    assert(0);
            }
    }
    if(NUM_INST<=instrIndex + 1) {
        RUN_BIT = FALSE;
    }
}
