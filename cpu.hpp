#include "memory.hpp"

class cpu{
	public:
		bool clock();
		void reset();
		//REGISTERS
		unsigned char  A;
		unsigned char  X;
		unsigned char  Y;
		unsigned char  SP;
		unsigned char  stat;
		unsigned short PC;
		cpu(memory m) {
			mem = m;
		};
		int clocks;
	private:
		memory mem;
};
