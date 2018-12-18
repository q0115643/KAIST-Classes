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
/* Purpose: Read instruction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) {
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

void FLUSH_IF_ID(){
    CURRENT_STATE.IF_ID_INST = 0;
    CURRENT_STATE.IF_ID_NPC = 0;
    CURRENT_STATE.IF_ID_COUNT = 0;
}
void FLUSH_ID_EX(){
    CURRENT_STATE.ID_EX_NPC = 0;
    CURRENT_STATE.ID_EX_REG1 = 0;
    CURRENT_STATE.ID_EX_REG2 = 0;
    CURRENT_STATE.ID_EX_IMM = 0;
    CURRENT_STATE.ID_EX_DEST = 0;
    CURRENT_STATE.ID_EX_ALUcontrol = 0; //for ALU
    CURRENT_STATE.ID_EX_ALUSrc = 0; //I type
    CURRENT_STATE.ID_EX_SHAMT = 0;
    CURRENT_STATE.ID_EX_MEM_WRITE = 0;
    CURRENT_STATE.ID_EX_BRANCH = 0;
    CURRENT_STATE.ID_EX_REG_WRITE = 0;
    CURRENT_STATE.ID_EX_MEM_READ = 0;
    CURRENT_STATE.ID_EX_FORWARDA=0;
    CURRENT_STATE.ID_EX_FORWARDB=0;
    CURRENT_STATE.ID_EX_FORARDV1=0;
    CURRENT_STATE.ID_EX_FORARDV2=0;
    CURRENT_STATE.ID_EX_RS=0;
    CURRENT_STATE.ID_EX_RT=0;
    CURRENT_STATE.ID_EX_COUNT = 0;
	CURRENT_STATE.ID_EX_JUMP_PC = 0;
}
void FLUSH_EX_MEM(){
    CURRENT_STATE.EX_MEM_NPC = 0;
    CURRENT_STATE.EX_MEM_ALU_OUT = 0;
    CURRENT_STATE.EX_MEM_W_VALUE = 0;
    CURRENT_STATE.EX_MEM_BR_TARGET = 0;
    CURRENT_STATE.EX_MEM_BR_TAKE = 0;
    CURRENT_STATE.EX_MEM_DEST = 0;
    CURRENT_STATE.EX_MEM_REG_WRITE = 0;
    CURRENT_STATE.EX_MEM_MEM_WRITE = 0;
    CURRENT_STATE.EX_MEM_MEM_READ = 0;
    CURRENT_STATE.EX_MEM_COUNT = 0;
	CURRENT_STATE.EX_MEM_JUMP_PC = 0;
}

void WB(){
    uint32_t data;
    if (CURRENT_STATE.MEM_WB_MEM_TO_REG) { //lw
        data = CURRENT_STATE.MEM_WB_MEM_OUT;
    } else {
        data = CURRENT_STATE.MEM_WB_ALU_OUT;
    }
    if (CURRENT_STATE.MEM_WB_REG_WRITE) {
        CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_DEST] = data;
    }
    if (CURRENT_STATE.MEM_WB_COUNT) {
        CURRENT_STATE.NUM_CLEAR++;
    }
}

void MEM(){
    CURRENT_STATE.MEM_WB_NPC = CURRENT_STATE.EX_MEM_NPC;
    uint32_t  MEM_WB_ALU_OUT = CURRENT_STATE.MEM_WB_ALU_OUT;
    uint32_t  MEM_WB_MEM_OUT = CURRENT_STATE.MEM_WB_MEM_OUT;
    CURRENT_STATE.MEM_WB_ALU_OUT = CURRENT_STATE.EX_MEM_ALU_OUT;
    uint32_t MEM_WB_DEST = CURRENT_STATE.MEM_WB_DEST;
    CURRENT_STATE.MEM_WB_DEST = CURRENT_STATE.EX_MEM_DEST;
    CURRENT_STATE.MEM_WB_BR_TAKE = CURRENT_STATE.EX_MEM_BR_TAKE;
    CURRENT_STATE.MEM_WB_BR_TARGET = CURRENT_STATE.EX_MEM_BR_TARGET;
    CURRENT_STATE.MEM_WB_REG_WRITE = CURRENT_STATE.EX_MEM_REG_WRITE;
    CURRENT_STATE.MEM_WB_COUNT = CURRENT_STATE.EX_MEM_COUNT;
    char MEM_WB_MEM_TO_REG = CURRENT_STATE.MEM_WB_MEM_TO_REG;
	CURRENT_STATE.MEM_WB_MEM_TO_REG = 0;
    if (CURRENT_STATE.EX_MEM_MEM_WRITE) { //sw
        mem_write_32(CURRENT_STATE.MEM_WB_ALU_OUT, CURRENT_STATE.EX_MEM_W_VALUE);
		CURRENT_STATE.MEM_WB_ALU_OUT = 0;
        CURRENT_STATE.MEM_WB_MEM_OUT = 0;
    }
    else if (CURRENT_STATE.EX_MEM_MEM_READ) {
        CURRENT_STATE.MEM_WB_MEM_TO_REG = 1;
        CURRENT_STATE.MEM_WB_MEM_OUT = mem_read_32(CURRENT_STATE.EX_MEM_ALU_OUT);
    }
    int forwarded = 0;
    if(MEM_WB_DEST == CURRENT_STATE.ID_EX_RS) {
        CURRENT_STATE.ID_EX_FORWARDA = 1;
        if (MEM_WB_MEM_TO_REG) {
            CURRENT_STATE.ID_EX_FORARDV1 = MEM_WB_MEM_OUT;
			printf("\nmem 스테이지 포워딩 장소1\n");
        }
        else {
            CURRENT_STATE.ID_EX_FORARDV1 = MEM_WB_ALU_OUT;
			printf("\nmem 스테이지 포워딩 장소2\n");
        }
        forwarded = 1;
    }
    if(MEM_WB_DEST == CURRENT_STATE.ID_EX_RT) {
        CURRENT_STATE.ID_EX_FORWARDB = 1;
        if (MEM_WB_MEM_TO_REG) {
            CURRENT_STATE.ID_EX_FORARDV2 = MEM_WB_MEM_OUT;
			printf("\nmem 스테이지 포워딩 장소3\n");
        }
        else {
            CURRENT_STATE.ID_EX_FORARDV2 = MEM_WB_ALU_OUT;
			printf("\nmem 스테이지 포워딩 장소4\n");
        }
        forwarded = 1;
    }
    if(CURRENT_STATE.EX_MEM_REG_WRITE) {
        if(CURRENT_STATE.EX_MEM_DEST == CURRENT_STATE.ID_EX_RS) {
            CURRENT_STATE.ID_EX_FORWARDA = 1;
            forwarded = 1;
            if (CURRENT_STATE.MEM_WB_MEM_TO_REG) {
				printf("\nmem 스테이지 포워딩 장소5\n");
                CURRENT_STATE.ID_EX_FORARDV1 = CURRENT_STATE.MEM_WB_MEM_OUT;
            }
            else {
				printf("\nmem 스테이지 포워딩 장소6\n");
                CURRENT_STATE.ID_EX_FORARDV1 = CURRENT_STATE.MEM_WB_ALU_OUT;
            }
        }
        if(CURRENT_STATE.EX_MEM_DEST == CURRENT_STATE.ID_EX_RT) {
            CURRENT_STATE.ID_EX_FORWARDB = 1;
            forwarded = 1;
            if (CURRENT_STATE.MEM_WB_MEM_TO_REG) {
				printf("\nmem 스테이지 포워딩 장소7\n");
                CURRENT_STATE.ID_EX_FORARDV2 = CURRENT_STATE.MEM_WB_MEM_OUT;
            }
            else {
				printf("\nmem 스테이지 포워딩 장소8\n");
                CURRENT_STATE.ID_EX_FORARDV2 = CURRENT_STATE.MEM_WB_ALU_OUT;
            }
        }
    }
    if(forwarded == 0) {
        CURRENT_STATE.ID_EX_FORWARDA = 0;
        CURRENT_STATE.ID_EX_FORWARDB = 0;
        CURRENT_STATE.ID_EX_FORARDV1 = 0;
        CURRENT_STATE.ID_EX_FORARDV2 = 0;
    }
}

void EX(){
    CURRENT_STATE.EX_MEM_NPC = CURRENT_STATE.ID_EX_NPC;
    CURRENT_STATE.EX_MEM_DEST = CURRENT_STATE.ID_EX_DEST;
    CURRENT_STATE.EX_MEM_MEM_WRITE = CURRENT_STATE.ID_EX_MEM_WRITE;
    CURRENT_STATE.EX_MEM_REG_WRITE = CURRENT_STATE.ID_EX_REG_WRITE;
    CURRENT_STATE.EX_MEM_MEM_READ = CURRENT_STATE.ID_EX_MEM_READ;
    CURRENT_STATE.EX_MEM_COUNT = CURRENT_STATE.ID_EX_COUNT;
	CURRENT_STATE.EX_MEM_JUMP_PC = CURRENT_STATE.ID_EX_JUMP_PC;
	printf("\nEX 스테이지에서 포워딩 전 REG1: 0x%0x, REG2: 0x%0x \n", CURRENT_STATE.ID_EX_REG1, CURRENT_STATE.ID_EX_REG2);
	if (CURRENT_STATE.ID_EX_FORWARDA == 1) {
        CURRENT_STATE.ID_EX_REG1 = CURRENT_STATE.ID_EX_FORARDV1;
		CURRENT_STATE.ID_EX_FORARDV1 = 0;
		CURRENT_STATE.ID_EX_FORWARDA = 0;
		printf("\n포워딩 reg1 value: 0x%0x\n", CURRENT_STATE.ID_EX_REG1);
    }
    if (CURRENT_STATE.ID_EX_FORWARDB == 1) {
        CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.ID_EX_FORARDV2;
		CURRENT_STATE.ID_EX_FORARDV2 = 0;
		CURRENT_STATE.ID_EX_FORWARDB = 0;
		printf("\n포워딩 reg2 value: 0x%0x\n", CURRENT_STATE.ID_EX_REG2);
	}
    switch(CURRENT_STATE.ID_EX_ALUcontrol){
        case 1: // add
            if(CURRENT_STATE.ID_EX_ALUSrc) {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 + CURRENT_STATE.ID_EX_IMM;
            }
            else {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 + CURRENT_STATE.ID_EX_REG2;
            }
            break;
        case 2: // subtract
            CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 - CURRENT_STATE.ID_EX_REG2;
            if(CURRENT_STATE.ID_EX_ALUSrc && CURRENT_STATE.EX_MEM_ALU_OUT==0 && CURRENT_STATE.ID_EX_BRANCH){
                CURRENT_STATE.EX_MEM_BR_TAKE = 1;
            }
            break;
        case 3: // AND
            if(CURRENT_STATE.ID_EX_ALUSrc) {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 & CURRENT_STATE.ID_EX_IMM;
            }
            else {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 & CURRENT_STATE.ID_EX_REG2;
            }
            break;
        case 4: // OR
            if(CURRENT_STATE.ID_EX_ALUSrc) {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 | CURRENT_STATE.ID_EX_IMM;
            }
            else {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 | CURRENT_STATE.ID_EX_REG2;
            }
            break;
        case 5: //slt
            if (CURRENT_STATE.ID_EX_ALUSrc) {
                if (CURRENT_STATE.ID_EX_REG1 < CURRENT_STATE.ID_EX_IMM) {
                    CURRENT_STATE.EX_MEM_ALU_OUT = 1;
                }
                else {
                    CURRENT_STATE.EX_MEM_ALU_OUT = 0;
                }
            }
            else {
                if (CURRENT_STATE.ID_EX_REG1 < CURRENT_STATE.ID_EX_REG2) {
                    CURRENT_STATE.EX_MEM_ALU_OUT = 1;
                }
                else {
                    CURRENT_STATE.EX_MEM_ALU_OUT = 0;
                }
            }
            break;
        case 6: //shift left
            CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG2 << CURRENT_STATE.ID_EX_SHAMT;
            break;
        case 7: //shift right
            CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG2 >> CURRENT_STATE.ID_EX_SHAMT;
            break;
        case 8: //NOR
            if(CURRENT_STATE.ID_EX_ALUSrc) {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 | CURRENT_STATE.ID_EX_IMM;
            }
            else {
                CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 | CURRENT_STATE.ID_EX_REG2;
            }
            CURRENT_STATE.EX_MEM_ALU_OUT = ~CURRENT_STATE.EX_MEM_ALU_OUT;
			break;
        case 9: //LUI
            CURRENT_STATE.EX_MEM_ALU_OUT = ((uint32_t)CURRENT_STATE.ID_EX_IMM) << 16;
            break;
        case 10:  //BNE
            CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_REG1 - CURRENT_STATE.ID_EX_REG2;
            if(CURRENT_STATE.EX_MEM_ALU_OUT!=0){
                CURRENT_STATE.EX_MEM_BR_TAKE = 1;
            }
            break;
        default:
            break;
    }
    if(CURRENT_STATE.ID_EX_MEM_WRITE) { //sw
        CURRENT_STATE.EX_MEM_W_VALUE = CURRENT_STATE.ID_EX_REG2;
    }
    if(CURRENT_STATE.ID_EX_BRANCH) {
        CURRENT_STATE.EX_MEM_BR_TARGET = CURRENT_STATE.ID_EX_NPC + (CURRENT_STATE.ID_EX_IMM << 2);
    }
	if(CURRENT_STATE.ID_EX_JUMP_PC) {
		FLUSH_IF_ID();
		CURRENT_STATE.PC = CURRENT_STATE.ID_EX_JUMP_PC;
		CURRENT_STATE.ID_EX_JUMP_PC = 0;
	}
}

void ID() {
	FLUSH_ID_EX();
    CURRENT_STATE.ID_EX_NPC = CURRENT_STATE.IF_ID_NPC;
    CURRENT_STATE.ID_EX_COUNT = CURRENT_STATE.IF_ID_COUNT;
    short opcode, func_code, imm;
	uint32_t rs, rt, rd;
    unsigned char shamt;
    uint32_t target;
    instruction *inst = CURRENT_STATE.IF_ID_INST;
    if(inst == 0){
        return;
    }
    opcode = OPCODE(inst);
    switch (opcode) {
        case 0x0:
            CURRENT_STATE.ID_EX_RS = RS(inst);
            CURRENT_STATE.ID_EX_RT = RT(inst);
            CURRENT_STATE.ID_EX_REG1 = CURRENT_STATE.REGS[RS(inst)];
            CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.REGS[RT(inst)];
            CURRENT_STATE.ID_EX_DEST = RD(inst);
            CURRENT_STATE.ID_EX_SHAMT = SHAMT(inst);
            CURRENT_STATE.ID_EX_ALUSrc = 0;
			CURRENT_STATE.ID_EX_REG_WRITE = 1;
			func_code = FUNC(inst);
			switch (func_code) {
                case 0x24:
                    //AND
                    CURRENT_STATE.ID_EX_ALUcontrol = 3;
                    break;
                case 0x21:
                    //ADDU
                    CURRENT_STATE.ID_EX_ALUcontrol = 1;
                    break;
                case 0x27:
                    //NOR
					CURRENT_STATE.ID_EX_ALUcontrol = 8;
                    break;
                case 0x25:
                    //OR
                    CURRENT_STATE.ID_EX_ALUcontrol = 4;
                    break;
                case 0x2b:
                    //SLTU
                    CURRENT_STATE.ID_EX_ALUcontrol = 5;
                    break;
                case 0x00:
                    //SLL
                    CURRENT_STATE.ID_EX_ALUcontrol = 6;
                    break;
                case 0x02:
                    //SRL
                    CURRENT_STATE.ID_EX_ALUcontrol = 7;
                    break;
                case 0x23:
                    //SUBU
                    CURRENT_STATE.ID_EX_ALUcontrol = 2;
                    break;
                case 0x08:
                    //JR
                    CURRENT_STATE.ID_EX_ALUcontrol = 0;
                    CURRENT_STATE.ID_EX_JUMP_PC = CURRENT_STATE.ID_EX_REG1;
                    CURRENT_STATE.ID_EX_REG_WRITE = 0;
                    break;
                default:
                    printf("Unknown function code type: %d\n", func_code);
            }
            break;

            //TYPE J
        case 0x2:        //(0x000010)J
            CURRENT_STATE.ID_EX_ALUcontrol = 0;
            target = (uint32_t) TARGET(inst);
            CURRENT_STATE.ID_EX_JUMP_PC = ((CURRENT_STATE.IF_ID_NPC - BYTES_PER_WORD) & 0xf0000000) | (target << 2);
            CURRENT_STATE.ID_EX_REG_WRITE = 0;
            break;
        case 0x3:        //(0x000011)JAL
            CURRENT_STATE.ID_EX_ALUcontrol = 0;
            target = (uint32_t) TARGET(inst);
            CURRENT_STATE.REGS[31] = CURRENT_STATE.ID_EX_NPC + 4;
            CURRENT_STATE.ID_EX_JUMP_PC = ((CURRENT_STATE.IF_ID_NPC - BYTES_PER_WORD) & 0xf0000000) | (target << 2);
            CURRENT_STATE.ID_EX_REG_WRITE = 0;
            break;

            //Type I
        default:
            CURRENT_STATE.ID_EX_RS = RS(inst);
            CURRENT_STATE.ID_EX_RT = RT(inst);
            CURRENT_STATE.ID_EX_REG1 = CURRENT_STATE.REGS[RS(inst)];
            CURRENT_STATE.ID_EX_REG2 = CURRENT_STATE.REGS[RT(inst)];
            CURRENT_STATE.ID_EX_IMM = IMM(inst);
			CURRENT_STATE.ID_EX_ALUSrc = 1;
			CURRENT_STATE.ID_EX_REG_WRITE = 1;
			switch (opcode) {
                case 0x9:        //(0x001001)ADDIU
                    CURRENT_STATE.ID_EX_ALUcontrol = 1;
                    CURRENT_STATE.ID_EX_DEST = CURRENT_STATE.ID_EX_RT;
                    break;
                case 0xc:        //(0x001100)ANDI
                    CURRENT_STATE.ID_EX_ALUcontrol = 3;
                    CURRENT_STATE.ID_EX_DEST = CURRENT_STATE.ID_EX_RT;
                    break;
                case 0xf:        //(0x001111)LUI
                    CURRENT_STATE.ID_EX_ALUcontrol = 9;
                    CURRENT_STATE.ID_EX_DEST = CURRENT_STATE.ID_EX_RT;
                    break;
                case 0xd:        //(0x001101)ORI
                    CURRENT_STATE.ID_EX_ALUcontrol = 4;
                    CURRENT_STATE.ID_EX_DEST = CURRENT_STATE.ID_EX_RT;
                    break;
                case 0xb:        //(0x001011)SLTIU
                    CURRENT_STATE.ID_EX_ALUcontrol = 5;
                    CURRENT_STATE.ID_EX_DEST = CURRENT_STATE.ID_EX_RT;
                    break;
                case 0x23:        //(0x100011)LW
                    CURRENT_STATE.ID_EX_ALUcontrol = 1;
                    CURRENT_STATE.ID_EX_MEM_READ = 1;
                    CURRENT_STATE.ID_EX_DEST = CURRENT_STATE.ID_EX_RT;
                    break;
                case 0x2b:        //(0x101011)SW
                    CURRENT_STATE.ID_EX_ALUcontrol = 1;
					CURRENT_STATE.ID_EX_REG_WRITE = 0;
					CURRENT_STATE.ID_EX_MEM_WRITE = 1;
                    break;
                case 0x4:        //(0x000100)BEQ
                    CURRENT_STATE.ID_EX_ALUcontrol = 2;
					CURRENT_STATE.ID_EX_REG_WRITE = 0;
					CURRENT_STATE.ID_EX_BRANCH = 1;
                    break;
                case 0x5:        //(0x000101)BNE
                    CURRENT_STATE.ID_EX_ALUcontrol = 10;
					CURRENT_STATE.ID_EX_REG_WRITE = 0;
					CURRENT_STATE.ID_EX_BRANCH = 1;
                    break;
                default:
                    printf("Unknown instruction type: %d\n", opcode);
            }
    }
}

void IF(){
    CURRENT_STATE.IF_ID_NPC = CURRENT_STATE.PC + BYTES_PER_WORD;
    CURRENT_STATE.IF_PC = CURRENT_STATE.PC;
    CURRENT_STATE.IF_ID_INST = get_inst_info(CURRENT_STATE.PC);
    CURRENT_STATE.IF_ID_COUNT = 1;
	CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;
    if (NUM_INST) {
        if (CURRENT_STATE.IF_PC < MEM_TEXT_START || CURRENT_STATE.IF_PC >= (MEM_TEXT_START + (NUM_INST * BYTES_PER_WORD))) {
            FLUSH_IF_ID();
        }
    }
	if (CURRENT_STATE.EX_MEM_MEM_READ) {
		if (CURRENT_STATE.ID_EX_DEST == CURRENT_STATE.ID_EX_RT) {
			if(CURRENT_STATE.ID_EX_RS == CURRENT_STATE.EX_MEM_DEST) {
				CURRENT_STATE.PC -= BYTES_PER_WORD;
				CURRENT_STATE.LOAD_USE = 1;
			}
		}
		else {
			if ((CURRENT_STATE.ID_EX_RS == CURRENT_STATE.EX_MEM_DEST || CURRENT_STATE.ID_EX_RT == CURRENT_STATE.EX_MEM_DEST)) {
				CURRENT_STATE.PC -= BYTES_PER_WORD;
				CURRENT_STATE.LOAD_USE = 1;
			}
		}
	}
}

void CHOOSE_PC(){
    if(CURRENT_STATE.MEM_WB_BR_TAKE == 1) {
        CURRENT_STATE.PC = CURRENT_STATE.MEM_WB_BR_TARGET;
        CURRENT_STATE.MEM_WB_BR_TAKE = 0;
        CURRENT_STATE.MEM_WB_BR_TARGET = 0;
        FLUSH_ID_EX();
        FLUSH_EX_MEM();
        FLUSH_IF_ID();
        CURRENT_STATE.PIPE[0] = 0;
        CURRENT_STATE.PIPE[1] = 0;
        CURRENT_STATE.PIPE[2] = 0;
    } else if(CURRENT_STATE.EX_MEM_JUMP_PC) {
		CURRENT_STATE.PIPE[1] = 0;
		CURRENT_STATE.PC = CURRENT_STATE.PC + BYTES_PER_WORD;
		CURRENT_STATE.EX_MEM_JUMP_PC = 0;
	}
    else{
		CURRENT_STATE.PC = CURRENT_STATE.PC + BYTES_PER_WORD;
    }
}

void STALL_FOR_LOADUSE(){
	FLUSH_EX_MEM();
	CURRENT_STATE.PIPE[2] = 0;
	CURRENT_STATE.PIPE[1] -= BYTES_PER_WORD;
	CURRENT_STATE.LOAD_USE = 0;
}

void TERMINATE_CONDITION(){
	if (NUM_INST) {
		if (CURRENT_STATE.IF_PC < MEM_TEXT_START || CURRENT_STATE.IF_PC >= (MEM_TEXT_START + (NUM_INST * BYTES_PER_WORD))) {
			CURRENT_STATE.PIPE[0] = 0;
			CURRENT_STATE.PC -= BYTES_PER_WORD;
		}
	}
	if (!CURRENT_STATE.PIPE[0]&&!CURRENT_STATE.PIPE[1]&&!CURRENT_STATE.PIPE[2]&&!CURRENT_STATE.PIPE[3]&&!CURRENT_STATE.MEM_WB_NPC) {
		RUN_BIT = FALSE;
	}
	if (CURRENT_STATE.NUM_CLEAR >= MAX_INSTRUCTION_NUM) {
		RUN_BIT = FALSE;
	}
}

void SET_PIPE(){
	for (int i = PIPE_STAGE - 1; i > 0; i--) {
		CURRENT_STATE.PIPE[i] = CURRENT_STATE.PIPE[i - 1];
	}
	CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/

void process_instruction() {
    /** Your implementation here */
	SET_PIPE();
    WB();
	MEM();
	if(CURRENT_STATE.LOAD_USE) {
		STALL_FOR_LOADUSE();
	}
	else {
		EX();
		ID();
		IF();
	}
    CHOOSE_PC();
	TERMINATE_CONDITION();
}
