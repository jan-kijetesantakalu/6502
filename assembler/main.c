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
	if (slre_match("([A-Z][A-Z][A-Z])", line, strlen(line), &SYM_cap, 1, SLRE_IGNORE_CASE) < 0)
		exit(-1);
	printf("found symbol: [%.*s]\n", SYM_cap.len, SYM_cap.ptr);
	
	struct slre_cap arg_cap;
	
	struct instruction instr = {{0},0,0};
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

	else {
			printf("error writing symbol: invalid symbol %s\n", instr.SYM);
	}
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
		line[strlen(line)-1]='\0';
		printf("parsing line:\n[\n%s\n]\n", line);
		struct instruction parsed_line = parse_line(line);
		printf("instruction %s, with mode %d, and data %d\n", parsed_line.SYM, parsed_line.mode, parsed_line.operand);
		write_instruction(parsed_line, binfp);
	}

	fclose(asmrawfp);
	return 0;
}

