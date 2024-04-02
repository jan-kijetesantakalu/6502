#include "cpu6502.h"
#include <iostream>
#include <stdio.h>


using namespace std;
void cpu6502::clock()
{
	BUS.write(100, 5);
	clocks++;
	printf("CPU CLOCK %d READ FROM BUS %d\n", clocks, BUS.read(100)); //DEBUG
	cycles_to_wait--;
	if (cycles_to_wait <= 0) {
		//DO EMULATOR THINGS
		unsigned char opcode = BUS.read(PC);
		//CALL APPROPRIATE ADDR_MODE
		switch (opcode) {
			default:
				break;
				//NOT IMPLEMENTED
		}
	}
};

void cpu6502::reset()
{
	A = 0;
	Y = 0;
	X = 0;
	PC = (BUS.read(0xFFFD) << 8) | BUS.read(0xFFFC); 
	SP = 0; //???
	SPMSB = 0; //???
	P = {0, 0, 0, 0, 0, 0, 0};
	
};

void cpu6502::abs_addr()
{
	//4.1 W65C02 DS
	operand_addr = BUS.read(PC++) | (BUS.read(PC++) << 8);
	cycles_to_wait += 3;
};

void cpu6502::abs_idx_ind()
{
	//4.2 ""
	operand_addr = (BUS.read(PC++)+X) | (BUS.read(PC++) << 8);	
	PC = BUS.read(operand_addr) | (BUS.read(operand_addr+1) << 8);  
	cycles_to_wait += 6;
};

void cpu6502::abs_idx_X()
{
	//4.3 ""
	operand_addr = (BUS.read(PC++)+X) | (BUS.read(PC++) << 8);	
	cycles_to_wait += 4;
};

void cpu6502::abs_idx_Y()
{
	//4.4 ""
	operand_addr = (BUS.read(PC++)+Y) | (BUS.read(PC++) << 8);	
	cycles_to_wait += 4;
};
