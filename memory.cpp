#include "memory.hpp"
unsigned char memory::read(unsigned short addr) {
	return data[addr];
}
void memory::write(unsigned short addr, unsigned char v) {
	data[addr] = v;
}
