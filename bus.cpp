#include "bus.h"
#include <iostream>

unsigned char bus::read(unsigned short addr)
{
	return memory[addr];
};

void bus::write(unsigned short addr, unsigned char data)
{
	memory[addr] = data;
};
