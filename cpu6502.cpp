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
