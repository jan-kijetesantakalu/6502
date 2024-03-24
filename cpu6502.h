#include "bus.h"
#define u_char unsigned char
class cpu6502 {
	public:
		void clock();
		cpu6502(bus B) {
			BUS = B;
			clocks = 0;
			A = 0;
			Y = 0;
			X = 0;
			PC = 0; //Reads FFFC & FFFD to define on reset
			SP = 0;
			SPMSB = 0;
			P = {0, 0, 0, 0, 0, 0, 0};
		}
		void reset();
	private:
		//BUS
		bus BUS;
		//REGISTERS
		unsigned char A;
		unsigned char Y;
		unsigned char X;
		unsigned short PC;
		unsigned char SP;
		bool SPMSB;
		struct { 
			bool C; //CARRY
			bool Z; //ZERO
			bool I; //Interupt 
			bool D; //Decimal
			bool B; //BRK
			bool V; //Overflow
			bool N; //Negative
		} P;
		//DEBUGGING / EMU
		int clocks;
		
		//ADRESSING, RETURNS OPCODE
		unsigned char operand;
		unsigned short operand_addr;
		
		unsigned char abs_addr();
		unsigned char abs_idx_ind();
		unsigned char abs_idx_X();
		unsigned char abs_idx_Y();
		unsigned char abs_ind_addr();
		unsigned char imp_A();
		unsigned char imm();
		unsigned char imp_i();
		u_char PC_rel();
		u_char stack();
		u_char zp();
		u_char zp_idx_ind();
		u_char zp_idx_X();
		u_char zp_idx_Y();
		u_char zp_ind_addr();
		u_char zp_ind_idx_Y();
};
