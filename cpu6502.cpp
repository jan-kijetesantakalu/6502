#include "cpu6502.h"
#include <iostream>
#include <stdio.h>

using namespace std;
void cpu6502::clock()
{
	BUS.write(100, 5);
	clocks++;
	printf("CPU CLOCK %d READ FROM BUS %d\n", clocks, BUS.read(100)); //DEBUG
};

void cpu6502::reset()
{
	A = 0;
	Y = 0;
	X = 0;
	PC = (BUS.read(0xFFFD) << 8) & BUS.read(0xFFFC); 
	SP = 0; //???
	SPMSB = 0; //???
	P = {0, 0, 0, 0, 0, 0, 0};
	
};
