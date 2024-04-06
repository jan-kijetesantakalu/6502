#include <stdio.h>
#include <stdbool.h>
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
};

void assemble(FILE *asmrawfp, FILE *binfp, int org, bool verb);
