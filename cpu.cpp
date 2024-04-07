#include "cpu.hpp"
#include "instructions.hpp"
#include <stdio.h>


void cpu::reset(bool verb) {
	//RESET
	if (verb)
		printf("Resetting\n");
	PC = (mem->read(0xFFFD) << 8) | mem->read(0xFFFC);
	if (verb)
		printf("Read %02X from FFFC, %02X from FFFD\nSet PC to %04X\n", PC&0xFF, PC>>8, PC);
	A = 0;
	X = 0;
	Y = 0;
	SP = 0xFD;
	stat = 1 << 5;
	//TAKES 8 CYCLES
	clocks = 0;
	clockwait = 8;
}

bool cpu::clock(bool verb) {
	//CLOCK
	if (verb)
		printf("Clocking\n");
	clockwait--;
	clocks++;
	if (clockwait <= 0) {
		opcode = mem->read(PC);
		PC++;
		stat = stat | (1 << 5);
		if (verb)
			printf("Read Opcode: %02X from %04X\n", opcode, PC-1);
		if (opcode == 0) {
			if (verb) 
				printf("BRK\n");
			return false; //BRK
			
		}
	}
	else {
		if (verb)
			printf("%d cycles remain from last instruction\n", clockwait);
	}
	return true; 
}
