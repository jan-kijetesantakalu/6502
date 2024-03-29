#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include "./lib/slre.h"
#include <stdlib.h>

#define LINE_SIZE 255



int parse_line(char *line, FILE* fp) {
	struct slre_cap SYM_cap;
	if (slre_match("([A-Z][A-Z][A-Z])", line, strlen(line), &SYM_cap, 1, SLRE_IGNORE_CASE) < 0)
		return -1;
	printf("found symbol: [%.*s]\n", SYM_cap.len, SYM_cap.ptr);
	
	struct slre_cap arg_cap;

	if (slre_match("#([\\$ % 0-9 A-F][0-9 A-F]*)", line, strlen(line), &arg_cap, 1, SLRE_IGNORE_CASE) > 0) {
		printf("found imm arg: [%.*s]\n", arg_cap.len, arg_cap.ptr);
	}

	return 0;
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
		printf("parsing line:\n[\n%s\n]\n", line);
		if (parse_line(line, binfp) < 0)
			printf("error parsing line\n");
	}

	fclose(asmrawfp);
	return 0;
}

