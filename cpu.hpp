#include "memory.hpp"

class cpu{
	public:
		bool clock(bool verb);
		void reset(bool verb);
		//REGISTERS
		unsigned char  A;
		unsigned char  X;
		unsigned char  Y;
		unsigned char  SP;
		unsigned char  stat;
		unsigned short PC;
		cpu(memory *m) {
			mem = m;
			reset(false);
		};
		int clocks;
		int clockwait;
		int opcode;
		//
		unsigned char operand;
		unsigned short operand_addr;
	private:
		memory *mem;
};
