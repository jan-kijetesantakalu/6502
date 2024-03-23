#include "cpu6502.h"
#include <iostream>
using namespace std;

void cpu6502::clock()
{
	clocks++;
	cout << "CPU CLOCK " << clocks;
};
