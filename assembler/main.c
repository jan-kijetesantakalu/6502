#include "assembler.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char **argv) {
	FILE *asmrawfp;
	FILE *binfp;
	if (argc == 1 || argc > 3) {
		printf("Usage: %s {input}.asm [{output}.bin]\n", argv[0]);
		return -1;
	}
	asmrawfp = fopen(argv[1], "r");
	if (asmrawfp == NULL) {
		printf("NULL FP\n");	
		return -1;
	}
	if (argc ==3) {	
		binfp = fopen(argv[2], "wb");
		if (binfp == NULL) {
			printf("NULL FP\n");
			return -1;
		}
	}
	else {
		binfp = fopen("asm.bin", "wb");
		if (binfp == NULL) {
			printf("NULL FP\n");
			return -1;
		}
	}
	assemble(asmrawfp, binfp, true);
	fclose(asmrawfp);
	fclose(binfp);
	return 0;
}
