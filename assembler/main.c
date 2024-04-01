#include <stdio.h>
#include <string.h>
#include "./lib/slre.h"
#include <stdlib.h>
#include <stdbool.h>

#define LINE_SIZE 64

enum mode {
	IMM,
	ABS,
	ZP_REL,
	IND,
	ABS_X,
	ABS_Y,
	ZP_X,
	ZP_Y,
	ZP_X_IND,
	ZP_IND_Y,
	IMP
};

struct instruction {
	char SYM[4];
	enum mode mode;
	unsigned short operand;
};


struct instruction parse_line(char *line) {
	struct slre_cap SYM_cap;
	struct instruction instr = {{0},0,0};
	
	unsigned char comment;

	comment = slre_match(";", line, strlen(line), NULL, 0, 0) -1;
	if (comment >= 0)
		line[comment] = '\0';	
	printf("removed comments: [%s]\n", line);

	struct slre_cap label_cap;
	
	unsigned char label;

	label = slre_match("([A-Z]+):", line, strlen(line), &label_cap, 1, SLRE_IGNORE_CASE);
	if (label >= 0) {
		//Matched label
		printf("found label: [%.*s]\n");
		
	}


	if (slre_match("([A-Z][A-Z][A-Z])", line, strlen(line), &SYM_cap, 1, SLRE_IGNORE_CASE) < 0) {
		instr.mode = -1;
		return instr;
	}
	printf("found symbol: [%.*s]\n", SYM_cap.len, SYM_cap.ptr);
	
	struct slre_cap arg_cap;
	
	memcpy(instr.SYM, SYM_cap.ptr, 3);
	
	//FIND ADDRESSIGN MODE WITH REGEX

	if (slre_match("\\s#(\\S*)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found imm: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = IMM;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S\\S\\S)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {	
		printf("found abs addr: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ABS;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found zp/rel: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_REL;
	}
	else if (slre_match("\\s\\((\\$?%?\\S\\S\\S\\S)\\)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found ind addr: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = IND;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S\\S\\S),X\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found abs idx X: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ABS_X;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S\\S\\S),Y\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found abs idx Y: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ABS_Y;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S),X\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found zp idx X: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_X;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S),Y\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found zp idx Y: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_Y;
	}
	else if (slre_match("\\s\\((\\$?%?\\S\\S),X\\)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found zp idx X ind: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_X_IND;
	}
	else if (slre_match("\\s\\((\\$?%?\\S\\S)\\),Y\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found zp ind idx Y: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_IND_Y;
	}
	else {
		printf("found no arguments, impl\n");
		instr.mode = IMP;
		return instr; //NO OPERAND PROCESSING REQUIRED
	}

	//PROCESS OPERAND STRING

	char *operand_str = calloc(sizeof(char), arg_cap.len);

	memcpy(operand_str, arg_cap.ptr, arg_cap.len);
	
	if (operand_str[0] == '$') { //hex
		instr.operand = (unsigned short)strtol(operand_str+1, NULL, 16);
	}
	else if (operand_str[0] == '%') { //bin
		instr.operand = (unsigned short)strtol(operand_str+1, NULL, 2);
	}
	else { //dec
		instr.operand = (unsigned short)strtol(operand_str, NULL, 10);
	}

	printf("opstr: %s -> val: %d\n", operand_str, instr.operand);
	
	free(operand_str);

	return instr;
}


#define s strhash

void write_instruction(struct instruction instr, FILE *fp) {
	if (strcmp("BRK", instr.SYM)==0) {
		//only ever impl
		fprintf(fp, "%c", 0x00);
	} 
	else if (strcmp("BPL", instr.SYM)==0){
		//only ever rel
		fprintf(fp, "%c", 0x10);
	}
	else if (strcmp("JSR", instr.SYM)==0){
		//only ever abs 
		fprintf(fp, "%c", 0x20);
	}
	else if (strcmp("BMI", instr.SYM)==0) {
		//only ever rel
		fprintf(fp, "%c", 0x30);
	}
	else if (strcmp("RTI", instr.SYM)==0) {
		//only ever impl
		fprintf(fp, "%c", 0x40);
	}
	else if (strcmp("BVC", instr.SYM)==0) {
		//only ever rel
		fprintf(fp, "%c", 0x50);
	}
	else if (strcmp("RTS", instr.SYM)==0) {
		//only ever impl
		fprintf(fp, "%c", 0x60);
	}
	else if (strcmp("BVS", instr.SYM)==0) {
		//only ever rel
		fprintf(fp, "%c", 0x70);
	}
	else if (strcmp("BCC", instr.SYM)==0) {
		//only ever rel
		fprintf(fp, "%c", 0x90);
	}
	else if (strcmp("LDY", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				fprintf(fp, "%c", 0xA0);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0xA4);
				break;
			case ABS:
				fprintf(fp, "%c", 0xAC);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0xB4);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0xBC);
				break;
			default:
				printf("error writing LDY: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BCS", instr.SYM)==0) {
		//only ever rel
		fprintf(fp, "%c", 0xB0);
	}
	else if (strcmp("CPY", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				fprintf(fp, "%c", 0xC0);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0xC4);
				break;
			case ABS:
				fprintf(fp, "%c", 0xCC);
				break;
			default:
				printf("error writing CPY: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BNE", instr.SYM)==0) {
		//only ever rel
		fprintf(fp, "%c", 0xD0);
	}
	else if (strcmp("CPX", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				fprintf(fp, "%c", 0xE0);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0xE4);
				break;
			case ABS:
				fprintf(fp, "%c", 0xEC);
				break;
			default:
				printf("error writing CPX: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BEQ", instr.SYM)==0) {
		//only ever rel
		fprintf(fp, "%c", 0xF0);
	}
	else if (strcmp("ORA", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0x01);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0x11);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0x05);
				break;
			case IMM:
				fprintf(fp, "%c", 0x09);
				break;
			case ABS:
				fprintf(fp, "%c", 0x0D);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x15);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0x19);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x1D);
				break;
			default:
				printf("error writing ORA: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("AND", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0x21);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0x31);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0x25);
				break;
			case IMM:
				fprintf(fp, "%c", 0x29);
				break;
			case ABS:
				fprintf(fp, "%c", 0x2D);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x35);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0x39);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x3D);
				break;
			default:
				printf("error writing AND: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("EOR", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0x41);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0x51);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0x45);
				break;
			case IMM:
				fprintf(fp, "%c", 0x49);
				break;
			case ABS:
				fprintf(fp, "%c", 0x4D);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x55);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0x59);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x5D);
				break;
			default:
				printf("error writing EOR: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("ADC", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0x61);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0x71);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0x65);
				break;
			case IMM:
				fprintf(fp, "%c", 0x69);
				break;
			case ABS:
				fprintf(fp, "%c", 0x6D);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x75);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0x79);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x7D);
				break;
			default:
				printf("error writing ADC: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("STA", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0x81);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0x91);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0x85);
				break;
			case ABS:
				fprintf(fp, "%c", 0x8D);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x95);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0x99);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x9D);
				break;
			default:
				printf("error writing STA: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("LDA", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0xA1);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0xB1);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0xA5);
				break;
			case IMM:
				fprintf(fp, "%c", 0xA9);
				break;
			case ABS:
				fprintf(fp, "%c", 0xAD);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0xB5);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0xB9);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0xBD);
				break;
			default:
				printf("error writing LDA: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("CMP", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0xC1);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0xD1);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0xC5);
				break;
			case IMM:
				fprintf(fp, "%c", 0xC9);
				break;
			case ABS:
				fprintf(fp, "%c", 0xCD);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0xD5);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0xD9);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0xDD);
				break;
			default:
				printf("error writing CMP: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("SBC", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				fprintf(fp, "%c", 0xE1);
				break;
			case ZP_IND_Y:
				fprintf(fp, "%c", 0xF1);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0xE5);
				break;
			case IMM:
				fprintf(fp, "%c", 0xE9);
				break;
			case ABS:
				fprintf(fp, "%c", 0xED);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0xF5);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0xF9);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0xFD);
				break;
			default:
				printf("error writing SBC: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("LDX", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				fprintf(fp, "%c", 0xA2);
				break;
			case ZP_REL:
				fprintf(fp, "%c", 0xA6);
				break;
			case ABS:
				fprintf(fp, "%c", 0xAE);
				break;
			case ZP_Y:
				fprintf(fp, "%c", 0xB6);
				break;
			case ABS_Y:
				fprintf(fp, "%c", 0xBE);
				break;
			default:
				printf("error writing LDX: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BIT", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0x24);
				break;
			case ABS:
				fprintf(fp, "%c", 0x2C);
				break;
			default:
				printf("error writing BIT: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("STY", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0x84);
				break;
			case ABS:
				fprintf(fp, "%c", 0x8C);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x94);
				break;
			default:
				printf("error writing STY: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("ASL", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0x06);
				break;
			case IMM:
				fprintf(fp, "%c", 0x0A);
				break;
			case ABS:
				fprintf(fp, "%c", 0x0E);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x16);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x1E);
				break;
			default:
				printf("error writing ASL: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("ROL", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0x26);
				break;
			case IMM:
				fprintf(fp, "%c", 0x2A);
				break;
			case ABS:
				fprintf(fp, "%c", 0x2E);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x36);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x3E);
				break;
			default:
				printf("error writing ROL: invalid mode %d", instr.mode);
				break;
		}	
	}	
	else if (strcmp("LSR", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0x46);
				break;
			case IMM:
				fprintf(fp, "%c", 0x4A);
				break;
			case ABS:
				fprintf(fp, "%c", 0x4E);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x56);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x5E);
				break;
			default:
				printf("error writing LSR: invalid mode %d", instr.mode);
				break;
		}	
	}	
	else if (strcmp("ROR", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0x66);
				break;
			case IMM:
				fprintf(fp, "%c", 0x6A);
				break;
			case ABS:
				fprintf(fp, "%c", 0x6E);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0x76);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0x7E);
				break;
			default:
				printf("error writing ROR: invalid mode %d", instr.mode);
				break;
		}	
	}	
	else if (strcmp("STX", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0x86);
				break;
			case ABS:
				fprintf(fp, "%c", 0x8E);
				break;
			case ZP_Y:
				fprintf(fp, "%c", 0x96);
				break;
			default:
				printf("error writing STX: invalid mode %d", instr.mode);
				break;
		}
		
	}
	else if (strcmp("DEC", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				fprintf(fp, "%c", 0xC6);
				break;
			case ABS:
				fprintf(fp, "%c", 0xCE);
				break;
			case ZP_X:
				fprintf(fp, "%c", 0xD6);
				break;
			case ABS_X:
				fprintf(fp, "%c", 0xDE);
				break;
			default:
				printf("error writing symbol DEC: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("INC", instr.SYM)==0) {
		switch (instr.mode) {
                        case ZP_REL:
                                fprintf(fp, "%c", 0xE6);
                                break;
                        case ABS:
                                fprintf(fp, "%c", 0xEE);
                                break;
                        case ZP_X:
                                fprintf(fp, "%c", 0xF6);
                                break;
                        case ABS_X:
                                fprintf(fp, "%c", 0xFE);
                                break;
                        default:
                                printf("error writing symbol INC: invalid mode %d", instr.mode);
                                break;
                }
	}
	else if (strcmp("PHP", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x08);
	}	
	else if (strcmp("CLC", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x18);
	}
	else if (strcmp("PLP", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x28);
	}
	else if (strcmp("SEC", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x38);
	}
	else if (strcmp("PHA", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x48);
	}
	else if (strcmp("CLI", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x58);
	}
	else if (strcmp("PLA", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x68);
	}
	else if (strcmp("SEI", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x78);
	}
	else if (strcmp("DEY", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x88);
	}
	else if (strcmp("TYA", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x98);
	}
	else if (strcmp("TAY", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xA8);
	}
	else if (strcmp("CLV", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xB8);
	}
	else if (strcmp("INY", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xC8);
	}
	else if (strcmp("CLD", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xD8);
	}
	else if (strcmp("INX", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xE8);
	}
	else if (strcmp("SED", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xF8);
	}
	else if (strcmp("TXA", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x8A);
	}
	else if (strcmp("TXS", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0x9A);
	}
	else if (strcmp("TAX", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xAA);
	}
	else if (strcmp("TSX", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xBA);
	}
	else if (strcmp("DEX", instr.SYM)==0) {
		//only ever IMP
		fprintf(fp, "%c", 0xCA);
	}
	else if (strcmp("NOP", instr.SYM)==0) {
		fprintf(fp, "%c", 0xEA);
	}
	else if (strcmp("JMP", instr.SYM)==0) {
		switch(instr.mode) {
			case ABS:
				fprintf(fp, "%c", 0x4C);
				break;
			case IND:
				fprintf(fp, "%c", 0x6C);
				break;
			default:
				printf("error writing JMP: invalid mode %d", instr.mode);
				break;
		}
	}
	
	else {
		printf("error writing symbol: invalid symbol %s\n", instr.SYM);
		return;
	}
	switch (instr.mode) {
		case ZP_REL:
		case ZP_X:
		case ZP_Y:
		case ZP_X_IND:
		case ZP_IND_Y:
		case IMM:
			fprintf(fp, "%c", instr.operand & 0xFF);
			break;
		case IND:
		case ABS_X:
		case ABS_Y:
		case ABS:
			fprintf(fp, "%c%c", instr.operand & 0xFF, (instr.operand>>8) & 0xFF);
			break;	
		case IMP:
			break;
		default:
			printf("error wrtiting in mode %d: invalid mode\n", instr.mode);
	}
	return;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: %s {input}.asm {output}.bin", argv[0]);
		return -1;
	}
	FILE *asmrawfp = fopen(argv[1], "r");
	if (asmrawfp == NULL) {
		printf("NULL FP");	
		return -1;
	}
	
	FILE *binfp = fopen(argv[2], "w");
	if (binfp == NULL) {
		printf("NULL FP");
		return -1;
	}

	char line[LINE_SIZE];
	while (fgets(line, LINE_SIZE+1, asmrawfp) != NULL) {
		if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1]='\0';
		printf("parsing line:\n[\n%s\n]\n", line);
		struct instruction parsed_line = parse_line(line);
		if (parsed_line.mode == -1) {
			printf("invalid line, continuing\n");
			continue;
		}
		printf("instruction %s, with mode %d, and data %d\n", parsed_line.SYM, parsed_line.mode, parsed_line.operand);
		write_instruction(parsed_line, binfp);
	}

	fclose(asmrawfp);
	return 0;
}

