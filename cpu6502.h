#include "bus.h"
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
		int cycles_to_wait;
		//ADRESSING, RETURNS OPCODE
		unsigned char operand;
		unsigned short operand_addr;
		void abs_addr();
		void abs_idx_ind();
		void abs_idx_X();
		void abs_idx_Y();
		void abs_ind_addr();
		void imp_A();
		void imm();
		void imp_i();
		void PC_rel();
		void stack();
		void zp();
		void zp_idx_ind();
		void zp_idx_X();
		void zp_idx_Y();
		void zp_ind_addr();
		void zp_ind_idx_Y();		
};
