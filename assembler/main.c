#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINE_SIZE 255


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
		print("NULL FP");
		return -1;
	}

	char line[LINE_SIZE];
	while (fgets(line, LINE_SIZE+1, asmrawfp) != NULL) {
        	printf("parsing: %s", line);
		parse_line(line, binfp);
	}

	fclose(asmrawfp);
	return 0;
}

int parse_line(char *line, FILE* fp) {
	
}
