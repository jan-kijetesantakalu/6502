#include <iostream>
#include "cpu6502.h"
using namespace std;

int main()
{
	cpu6502 CPU;
	while (1) {
		CPU.clock();
	}
	//cout << "Hello World!";
	return 0;
}
