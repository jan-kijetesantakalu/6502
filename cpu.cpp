#include "cpu.hpp"
#include <stdio.h>

void cpu::reset(bool verb) {
	//RESET
	PC = (mem->read(0xFFFD) << 8) | mem->read(0xFFFC);
	if (verb)
		printf("Set PC to %04X\n", PC);
	A = 0;
	X = 0;
	Y = 0;
	SP = 0xFD;
	stat = 1 << 5;
	//TAKES 8 CYCLES
	clockwait = 8;
}

bool cpu::clock(bool verb) {
	//CLOCK
	clocks++;
	if (clockwait <= 0) {
		opcode = mem->read(PC);
		PC++;
		stat = stat | (1 << 5);
		if (verb)
			printf("Read Opcode: %02X from %04X\n", opcode, PC-1);
		if (opcode == 0) {
			return false; //BRK
		}
	}
	else {
		printf("%d cycles remain from last instruction\n", clockwait);
	}
	clockwait--;
	return true; 
}
