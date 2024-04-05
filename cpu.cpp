#include "cpu.hpp"

void cpu::reset() {
	//RESET
}

bool cpu::clock() {
	//CLOCK
	clocks++;
	if (clocks > 5)
		return false; //BRK, return true if not BRK
	return true;
}
