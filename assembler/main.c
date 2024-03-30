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


struct instruction parse_line(char *line, FILE* fp) {
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

	return instr;
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
		struct instruction parsed_line = parse_line(line, binfp);
		printf("instruction %s, with mode %d, and data %d\n", parsed_line.SYM, parsed_line.mode, parsed_line.operand);
	}

	fclose(asmrawfp);
	return 0;
}

