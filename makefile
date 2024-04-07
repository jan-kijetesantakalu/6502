make main.cpp memory.cpp memory.hpp cpu.cpp cpu.hpp assembler/assembler.h assembler.main.c assembler/lib/slre.h assembler/lib/slre.c instructions.cpp instructions.hpp:
	g++ -o 6502 -g main.cpp memory.cpp cpu.cpp assembler/assembler.c assembler/lib/slre.c instructions.cpp 
