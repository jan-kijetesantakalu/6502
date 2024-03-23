#include <iostream>
#include "cpu6502.h" //CPU includes bus

using namespace std;

int main()
{
	bus BUS; 

	cpu6502 CPU = cpu6502(BUS);
		
	while (1) {
		CPU.clock();
	}
	//cout << "Hello World!";
	return 0;
}
