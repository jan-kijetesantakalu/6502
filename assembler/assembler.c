#include <stdio.h>
#include <string.h>
#include "./lib/slre.h"
#include <stdlib.h>
#include <stdbool.h>

#define LINE_SIZE 256

enum mode {
	ERROR = -1,
	IMM = 0,
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

struct label { //e.g. loop: *****
	char *id; //"loop"
	int offset;
	bool bitlatch;
};

struct scope {
	struct label defn[255];
	struct label inst[255];
	int defns;
	int insts;
};


unsigned char get_opcode(struct instruction instr, bool verb) {
	if (strcmp("BRK", instr.SYM)==0) {
		//only ever impl
		return ( 0x00);
	} 
	else if (strcmp("BPL", instr.SYM)==0){
		//only ever rel
		return ( 0x10);
	}
	else if (strcmp("JSR", instr.SYM)==0){
		//only ever abs 
		return ( 0x20);
	}
	else if (strcmp("BMI", instr.SYM)==0) {
		//only ever rel
		return ( 0x30);
	}
	else if (strcmp("RTI", instr.SYM)==0) {
		//only ever impl
		return ( 0x40);
	}
	else if (strcmp("BVC", instr.SYM)==0) {
		//only ever rel
		return ( 0x50);
	}
	else if (strcmp("RTS", instr.SYM)==0) {
		//only ever impl
		return ( 0x60);
	}
	else if (strcmp("BVS", instr.SYM)==0) {
		//only ever rel
		return ( 0x70);
	}
	else if (strcmp("BCC", instr.SYM)==0) {
		//only ever rel
		return ( 0x90);
	}
	else if (strcmp("LDY", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				return ( 0xA0);
				break;
			case ZP_REL:
				return ( 0xA4);
				break;
			case ABS:
				return ( 0xAC);
				break;
			case ZP_X:
				return ( 0xB4);
				break;
			case ABS_X:
				return ( 0xBC);
				break;
			default:
				if (verb) printf("error writing LDY: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BCS", instr.SYM)==0) {
		//only ever rel
		return ( 0xB0);
	}
	else if (strcmp("CPY", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				return ( 0xC0);
				break;
			case ZP_REL:
				return ( 0xC4);
				break;
			case ABS:
				return ( 0xCC);
				break;
			default:
				if (verb) printf("error writing CPY: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BNE", instr.SYM)==0) {
		//only ever rel
		return ( 0xD0);
	}
	else if (strcmp("CPX", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				return ( 0xE0);
				break;
			case ZP_REL:
				return ( 0xE4);
				break;
			case ABS:
				return ( 0xEC);
				break;
			default:
				if (verb) printf("error writing CPX: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BEQ", instr.SYM)==0) {
		//only ever rel
		return ( 0xF0);
	}
	else if (strcmp("ORA", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0x01);
				break;
			case ZP_IND_Y:
				return ( 0x11);
				break;
			case ZP_REL:
				return ( 0x05);
				break;
			case IMM:
				return ( 0x09);
				break;
			case ABS:
				return ( 0x0D);
				break;
			case ZP_X:
				return ( 0x15);
				break;
			case ABS_Y:
				return ( 0x19);
				break;
			case ABS_X:
				return ( 0x1D);
				break;
			default:
				if (verb) printf("error writing ORA: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("AND", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0x21);
				break;
			case ZP_IND_Y:
				return ( 0x31);
				break;
			case ZP_REL:
				return ( 0x25);
				break;
			case IMM:
				return ( 0x29);
				break;
			case ABS:
				return ( 0x2D);
				break;
			case ZP_X:
				return ( 0x35);
				break;
			case ABS_Y:
				return ( 0x39);
				break;
			case ABS_X:
				return ( 0x3D);
				break;
			default:
				if (verb) printf("error writing AND: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("EOR", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0x41);
				break;
			case ZP_IND_Y:
				return ( 0x51);
				break;
			case ZP_REL:
				return ( 0x45);
				break;
			case IMM:
				return ( 0x49);
				break;
			case ABS:
				return ( 0x4D);
				break;
			case ZP_X:
				return ( 0x55);
				break;
			case ABS_Y:
				return ( 0x59);
				break;
			case ABS_X:
				return ( 0x5D);
				break;
			default:
				if (verb) printf("error writing EOR: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("ADC", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0x61);
				break;
			case ZP_IND_Y:
				return ( 0x71);
				break;
			case ZP_REL:
				return ( 0x65);
				break;
			case IMM:
				return ( 0x69);
				break;
			case ABS:
				return ( 0x6D);
				break;
			case ZP_X:
				return ( 0x75);
				break;
			case ABS_Y:
				return ( 0x79);
				break;
			case ABS_X:
				return ( 0x7D);
				break;
			default:
				if (verb) printf("error writing ADC: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("STA", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0x81);
				break;
			case ZP_IND_Y:
				return ( 0x91);
				break;
			case ZP_REL:
				return ( 0x85);
				break;
			case ABS:
				return ( 0x8D);
				break;
			case ZP_X:
				return ( 0x95);
				break;
			case ABS_Y:
				return ( 0x99);
				break;
			case ABS_X:
				return ( 0x9D);
				break;
			default:
				if (verb) printf("error writing STA: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("LDA", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0xA1);
				break;
			case ZP_IND_Y:
				return ( 0xB1);
				break;
			case ZP_REL:
				return ( 0xA5);
				break;
			case IMM:
				return ( 0xA9);
				break;
			case ABS:
				return ( 0xAD);
				break;
			case ZP_X:
				return ( 0xB5);
				break;
			case ABS_Y:
				return ( 0xB9);
				break;
			case ABS_X:
				return ( 0xBD);
				break;
			default:
				if (verb) printf("error writing LDA: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("CMP", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0xC1);
				break;
			case ZP_IND_Y:
				return ( 0xD1);
				break;
			case ZP_REL:
				return ( 0xC5);
				break;
			case IMM:
				return ( 0xC9);
				break;
			case ABS:
				return ( 0xCD);
				break;
			case ZP_X:
				return ( 0xD5);
				break;
			case ABS_Y:
				return ( 0xD9);
				break;
			case ABS_X:
				return ( 0xDD);
				break;
			default:
				if (verb) printf("error writing CMP: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("SBC", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_X_IND:
				return ( 0xE1);
				break;
			case ZP_IND_Y:
				return ( 0xF1);
				break;
			case ZP_REL:
				return ( 0xE5);
				break;
			case IMM:
				return ( 0xE9);
				break;
			case ABS:
				return ( 0xED);
				break;
			case ZP_X:
				return ( 0xF5);
				break;
			case ABS_Y:
				return ( 0xF9);
				break;
			case ABS_X:
				return ( 0xFD);
				break;
			default:
				if (verb) printf("error writing SBC: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("LDX", instr.SYM)==0) {
		switch (instr.mode) {
			case IMM:
				return ( 0xA2);
				break;
			case ZP_REL:
				return ( 0xA6);
				break;
			case ABS:
				return ( 0xAE);
				break;
			case ZP_Y:
				return ( 0xB6);
				break;
			case ABS_Y:
				return ( 0xBE);
				break;
			default:
				if (verb) printf("error writing LDX: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("BIT", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0x24);
				break;
			case ABS:
				return ( 0x2C);
				break;
			default:
				if (verb) printf("error writing BIT: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("STY", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0x84);
				break;
			case ABS:
				return ( 0x8C);
				break;
			case ZP_X:
				return ( 0x94);
				break;
			default:
				if (verb) printf("error writing STY: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("ASL", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0x06);
				break;
			case IMP:
				return ( 0x0A);
				break;
			case ABS:
				return ( 0x0E);
				break;
			case ZP_X:
				return ( 0x16);
				break;
			case ABS_X:
				return ( 0x1E);
				break;
			default:
				if (verb) printf("error writing ASL: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("ROL", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0x26);
				break;
			case IMP:
				return ( 0x2A);
				break;
			case ABS:
				return ( 0x2E);
				break;
			case ZP_X:
				return ( 0x36);
				break;
			case ABS_X:
				return ( 0x3E);
				break;
			default:
				if (verb) printf("error writing ROL: invalid mode %d", instr.mode);
				break;
		}	
	}	
	else if (strcmp("LSR", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0x46);
				break;
			case IMP:
				return ( 0x4A);
				break;
			case ABS:
				return ( 0x4E);
				break;
			case ZP_X:
				return ( 0x56);
				break;
			case ABS_X:
				return ( 0x5E);
				break;
			default:
				if (verb) printf("error writing LSR: invalid mode %d", instr.mode);
				break;
		}	
	}	
	else if (strcmp("ROR", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0x66);
				break;
			case IMP:
				return ( 0x6A);
				break;
			case ABS:
				return ( 0x6E);
				break;
			case ZP_X:
				return ( 0x76);
				break;
			case ABS_X:
				return ( 0x7E);
				break;
			default:
				if (verb) printf("error writing ROR: invalid mode %d", instr.mode);
				break;
		}	
	}	
	else if (strcmp("STX", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0x86);
				break;
			case ABS:
				return ( 0x8E);
				break;
			case ZP_Y:
				return ( 0x96);
				break;
			default:
				if (verb) printf("error writing STX: invalid mode %d", instr.mode);
				break;
		}
		
	}
	else if (strcmp("DEC", instr.SYM)==0) {
		switch (instr.mode) {
			case ZP_REL:
				return ( 0xC6);
				break;
			case ABS:
				return ( 0xCE);
				break;
			case ZP_X:
				return ( 0xD6);
				break;
			case ABS_X:
				return ( 0xDE);
				break;
			default:
				if (verb) printf("error writing symbol DEC: invalid mode %d", instr.mode);
				break;
		}
	}
	else if (strcmp("INC", instr.SYM)==0) {
		switch (instr.mode) {
                        case ZP_REL:
                                return ( 0xE6);
                                break;
                        case ABS:
                                return ( 0xEE);
                                break;
                        case ZP_X:
                                return ( 0xF6);
                                break;
                        case ABS_X:
                                return ( 0xFE);
                                break;
                        default:
                                if (verb) printf("error writing symbol INC: invalid mode %d", instr.mode);
                                break;
                }
	}
	else if (strcmp("PHP", instr.SYM)==0) {
		//only ever IMP
		return ( 0x08);
	}	
	else if (strcmp("CLC", instr.SYM)==0) {
		//only ever IMP
		return ( 0x18);
	}
	else if (strcmp("PLP", instr.SYM)==0) {
		//only ever IMP
		return ( 0x28);
	}
	else if (strcmp("SEC", instr.SYM)==0) {
		//only ever IMP
		return ( 0x38);
	}
	else if (strcmp("PHA", instr.SYM)==0) {
		//only ever IMP
		return ( 0x48);
	}
	else if (strcmp("CLI", instr.SYM)==0) {
		//only ever IMP
		return ( 0x58);
	}
	else if (strcmp("PLA", instr.SYM)==0) {
		//only ever IMP
		return ( 0x68);
	}
	else if (strcmp("SEI", instr.SYM)==0) {
		//only ever IMP
		return ( 0x78);
	}
	else if (strcmp("DEY", instr.SYM)==0) {
		//only ever IMP
		return ( 0x88);
	}
	else if (strcmp("TYA", instr.SYM)==0) {
		//only ever IMP
		return ( 0x98);
	}
	else if (strcmp("TAY", instr.SYM)==0) {
		//only ever IMP
		return ( 0xA8);
	}
	else if (strcmp("CLV", instr.SYM)==0) {
		//only ever IMP
		return ( 0xB8);
	}
	else if (strcmp("INY", instr.SYM)==0) {
		//only ever IMP
		return ( 0xC8);
	}
	else if (strcmp("CLD", instr.SYM)==0) {
		//only ever IMP
		return ( 0xD8);
	}
	else if (strcmp("INX", instr.SYM)==0) {
		//only ever IMP
		return ( 0xE8);
	}
	else if (strcmp("SED", instr.SYM)==0) {
		//only ever IMP
		return ( 0xF8);
	}
	else if (strcmp("TXA", instr.SYM)==0) {
		//only ever IMP
		return ( 0x8A);
	}
	else if (strcmp("TXS", instr.SYM)==0) {
		//only ever IMP
		return ( 0x9A);
	}
	else if (strcmp("TAX", instr.SYM)==0) {
		//only ever IMP
		return ( 0xAA);
	}
	else if (strcmp("TSX", instr.SYM)==0) {
		//only ever IMP
		return ( 0xBA);
	}
	else if (strcmp("DEX", instr.SYM)==0) {
		//only ever IMP
		return ( 0xCA);
	}
	else if (strcmp("NOP", instr.SYM)==0) {
		return ( 0xEA);
	}
	else if (strcmp("JMP", instr.SYM)==0) {
		switch(instr.mode) {
			case ABS:
				return ( 0x4C);
				break;
			case IND:
				return ( 0x6C);
				break;
			default:
				if (verb) printf("error writing JMP: invalid mode %d", instr.mode);
				break;
		}
	}
	
	else {
		if (verb) printf("error writing symbol: invalid symbol %s\n", instr.SYM);
	}
	return 0x00;
}


struct instruction parse_line(char *line, FILE *fp, struct scope *scope, int org, bool verb) {
	struct slre_cap SYM_cap;
	struct instruction instr = {{0},ERROR,0};
	
	int comment;

	comment = slre_match(";", line, strlen(line), NULL, 0, 0) -1;
	if (comment >= 0)
		line[comment] = '\0';	
	if (verb) printf("removed comments: [%s]\n", line);

	struct slre_cap label_cap;
	
	int label = slre_match("([A-Z_-]+):", line, strlen(line), &label_cap, 1, SLRE_IGNORE_CASE);
	if (label >= 0) {
		//Matched label
		fseek(fp, 0, SEEK_END);
		scope->defn[scope->defns].id = (char *)calloc((label_cap.len+1), sizeof(char));
		scope->defn[scope->defns].offset = ftell(fp)+org;
		memcpy(scope->defn[scope->defns].id, label_cap.ptr, label_cap.len);
		scope->defns++;
		if (verb) printf("defined label %d -> [[%s]->[%d]] | next defn no: %d\n", scope->defns-1, scope->defn[scope->defns-1].id, scope->defn[scope->defns-1].offset, scope->defns);
		instr.mode = ERROR;
		return instr;		
	}
	

	if (slre_match("([A-Z][A-Z][A-Z])", line, strlen(line), &SYM_cap, 1, SLRE_IGNORE_CASE) < 0) {
		instr.mode = ERROR;
		return instr;
	}
	
	if (verb) printf("found symbol: [%.*s]\n", SYM_cap.len, SYM_cap.ptr);
	
	struct slre_cap arg_cap;
	
	memcpy(instr.SYM, SYM_cap.ptr, 3);
	
	instr.SYM[0] = instr.SYM[0] & 223;
	instr.SYM[1] = instr.SYM[1] & 223;
	instr.SYM[2] = instr.SYM[2] & 223;

	if (verb) printf("symbol: [%s]\n", instr.SYM);

	//FIND ADDRESSIGN MODE WITH REGEX
	
	if (slre_match("[A-Z][A-Z][A-Z]\\s+([A-Z_-]+)\\s*$", line , strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found label [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ABS;
		scope->inst[scope->insts].bitlatch = false;
		if (((get_opcode(instr, verb) & 0x0F) == 0) && (get_opcode(instr, verb) != 0x20)){ 
			if (verb) printf("using mode ZP_REL\n");
			instr.mode = ZP_REL;
			scope->inst[scope->insts].bitlatch = true;
		}
		scope->inst[scope->insts].id = (char *)calloc((arg_cap.len+1),sizeof(char));
		scope->inst[scope->insts].offset = ftell(fp)+1;
		memcpy(scope->inst[scope->insts].id, arg_cap.ptr, arg_cap.len);
		scope->insts++;
		if (verb) printf("instanced label %d : [[%s]->[%d]] | next instance no: %d\n", scope->insts-1, scope->inst[scope->insts-1].id, scope->inst[scope->insts-1].offset, scope->insts);
	}
	else if (slre_match("\\s#(\\S*)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found imm: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = IMM;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found zp/rel: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_REL;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S\\S\\S),X\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found abs idx X: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ABS_X;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S\\S\\S),Y\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found abs idx Y: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ABS_Y;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S),X\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found zp idx X: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_X;
	}
	else if (slre_match("\\s(\\$?%?\\S\\S),Y\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found zp idx Y: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_Y;
	}
	else if (slre_match("\\s\\((\\$?%?\\S\\S),X\\)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found zp idx X ind: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_X_IND;
	}
	else if (slre_match("\\s\\((\\$?%?\\S\\S)\\),Y\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		if (verb) printf("found zp ind idx Y: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ZP_IND_Y;
	}
	else if (slre_match("\\s\\((\\$?%?\\S\\S\\S\\S)\\)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) { //if check earlier then will match ,X as \\S\\S
		if (verb) printf("found ind addr: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = IND;
	}
	else if (slre_match("\\s([\\$%]\\S+)\\s*$", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {	//as above
		if (verb) printf("found abs addr: [%.*s]\n", arg_cap.len, arg_cap.ptr);
		instr.mode = ABS;
	}
	else {
		if (verb) printf("found no arguments, impl\n");
		instr.mode = IMP;
		return instr; //NO OPERAND PROCESSING REQUIRED
	}

	//PROCESS OPERAND STRING

	char *operand_str = (char *)calloc(sizeof(char), arg_cap.len);

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

	if (verb) printf("opstr: %s -> val: %d\n", operand_str, instr.operand);
	
	free(operand_str);

	return instr;
}


#define s strhash
	
void write_instruction(struct instruction instr, FILE *fp, bool verb) {
	fprintf(fp, "%c", get_opcode(instr, verb));
	switch (instr.mode) {
		case ZP_REL:
		case ZP_X:
		case ZP_Y:
		case ZP_X_IND:
		case ZP_IND_Y:
		case IMM:
			fprintf (fp, "%c", instr.operand & 0xFF);
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
			if (verb) printf("error wrtiting in mode %d: invalid mode\n", instr.mode);
	}
	return;
}


void assemble(FILE *asmrawfp, FILE *binfp, int org, bool verb) {
	char line[LINE_SIZE];
	struct scope scope = {{{}},{{}},0, 0};

	while (fgets(line, LINE_SIZE, asmrawfp) != NULL) {
		if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1]='\0';
		if (verb) printf("parsing line:\n[\n%s\n]\n", line);
		struct instruction parsed_line = parse_line(line, binfp, &scope, org, verb);
		if (parsed_line.mode == -1)
			continue;
		if (verb) printf("instruction %s, with mode %d, and data %d\n", parsed_line.SYM, parsed_line.mode, parsed_line.operand);
		write_instruction(parsed_line, binfp, verb);
	}


	if (verb) printf("\nLABEL POSTPROCESSING:\n");
	
	for (int i = 0; i < scope.insts; ++i) {
		if (verb) printf("label instance [%d]: id [%s] @ [%d] %s\n", i, scope.inst[i].id, scope.inst[i].offset, scope.inst[i].bitlatch ? "1 bit" : "2 bit");
		for (int d = 0; d < scope.defns; ++d) {
			if (strcmp(scope.defn[d].id, scope.inst[i].id)==0) {
				if (verb) printf("matched instance [%d] with definition [%d] : [%s] -> [%d]\n", i, d, scope.defn[d].id, scope.defn[d].offset);
				if (!scope.inst[i].bitlatch) {
					if (verb) printf("writing two bits");
					fseek(binfp, scope.inst[i].offset, SEEK_SET);
					unsigned char data[2] = {(unsigned char)((scope.defn[d].offset) & 0xFF), (unsigned char)((scope.defn[d].offset >>8) & 0xFF)};
					fwrite(data, sizeof(char), 2, binfp);
					if (verb) printf("written [%02x%02x] to [%d]\n", data[0], data[1], scope.inst[i].offset);
				}
				else {
					if (verb) printf("writing one bit");
					fseek(binfp, scope.inst[i].offset, SEEK_SET);
					unsigned char data = scope.defn[d].offset - scope.inst[i].offset - 1;
					fwrite(&data, sizeof(char), 1, binfp);
					if (verb) printf("written [%02x] to [%d]\n", data, scope.inst[i].offset);
				}
			}
		}
	}
	if (verb) printf("COMPLETE\n");
}

