#include "cpu.hpp"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include "assembler/assembler.h"

void dumpmem(memory& mem, unsigned short addr, unsigned short len) {
	int i;
	for (i = 0; i < len; i++) {
		if (((i % 8) == 0) && (i > 0)) 
			printf("\n");
		printf("%02X ", mem.read(i+addr));
	}
	if ((i % 16) != 0) {
		printf("\n");
	}
	printf("\n");
}

void loadprog(memory& memory, FILE *file, bool verb) {
	if (verb)
		printf("Copying file into memory:\n");
	fseek(file, 0, SEEK_END);
	unsigned short fsize = ftell(file);
	for (unsigned short i = 0; i < fsize; i++) {
		fseek(file, i, SEEK_SET);
		memory.write(i, getc(file));
	}
	if (verb) 
		dumpmem(memory, 0, fsize);
}

int prompt(cpu& cpu, memory& memory, bool verb) {
	printf(">> ");
	//VAL -> 65 | 0x65
	//dumpreg
	//setreg    REG      VAL
	//dumpmem   [LENGTH] [FROM]
	//setmem    ADDR     VAL
	//step	    [n]
	//exit
	//run
	//reset
	//loadprog
	
	char input[64] = {0};
	fgets(input, 64, stdin);
	
	char command[64] = {0};

	sscanf(input, "%s", command);
	
	for (int i = 0; i < strlen(command); i++) {
		command[i] = toupper(command[i]);
	}
	
	if (strcmp("HELP", command) == 0) {
		printf("Commands:\n\tHELP\t\t\t\t- Displays this message\n\tLOADPROG FILE\t\t\t- Loads program from FILE into 0000\n\tASM FILE\t\t\t- Assembles FILE and then loads into memory at 0000\n\tDUMPREG\t\t\t\t- Prints all registers\n\tSETREG REG [VAL]\t\t- Sets a Register to VAL (default 0)\n\tDUMPMEM [FROM] [LENGTH]\t\t- Prints LEGNTH (16) bytes of memory from FROM (0000)\n\tSETMEM ADDR VAL\t\t\t- Sets the memory at ADDR to VAL\n\tSTEP [N]\t\t\t- Runs N clock cycles\n\tRUN\t\t\t\t- Runs the program, disabling this prompt\n\tRESET\t\t\t\t- Resets the CPU\n\tEXIT\t\t\t\t- Exits\n\n");
	}
	else if (strcmp("STEP", command) == 0) {
		int n = 1;
		sscanf(input, "%s%d", command, &n);
		if (verb)
			printf("Stepping %d times\n", n);
		return n;
	}
	else if (strcmp("LOADPROG", command) == 0) {
		char prog[64] = {0};
		sscanf(input, "%s%s", command, prog);
		if (verb) {
			printf("Opening file [%s]\n", prog);
		}
		FILE *file = fopen(prog, "rb");
		if (file == NULL) {
			printf("File %s not found\n", prog);
			return 0;
		}
		loadprog(memory, file, verb);
	}
	else if (strcmp("ASM", command) == 0) {
		char prog[64] = {0};
		sscanf(input, "%s%s", command, prog);
		if (verb) {
			printf("Opening file [%s]\n", prog);
		}
		FILE *file = fopen(prog, "rb");
		if (file == NULL) {
			printf("File %s not found\n", prog);
			return 0;
		}
		FILE *bin = fopen("tmp.bin.tmp", "wb");
		assemble(file, bin, verb);
		fclose(bin);
		bin = fopen("tmp.bin.tmp", "rb");
		loadprog(memory, bin, verb);
		fclose(bin);
		fclose(file);
		remove("tmp.bin.tmp");
		
	}
	else if (strcmp("RESET", command) == 0) {
		cpu.reset();
		if (verb) {
			printf("Reset CPU\n");
			printf("Cycles: %d; PC: %04X; A: %02X; X: %02X; Y: %02X; SP: %02X; STAT: ", cpu.clocks, cpu.PC, cpu.A, cpu.X, cpu.Y, cpu.SP);
			std::bitset<8> stat(cpu.stat);
			std::cout << stat << '\n';	
		
		}
	}
	else if (strcmp("DUMPREG", command) == 0) {
		printf("Cycles: %d; PC: %04X; A: %02X; X: %02X; Y: %02X; SP: %02X; STAT: ", cpu.clocks, cpu.PC, cpu.A, cpu.X, cpu.Y, cpu.SP);
		std::bitset<8> stat(cpu.stat);
		std::cout << stat << '\n';	
	}
	else if (strcmp("SETREG", command) == 0) {
		char reg[64] = {0};
		char sval[64] = {0};
		sscanf(input, "%s%s%s", command, reg, sval);
		
		for (int i = 0; i < strlen(reg); i++) {
			reg[i] = toupper(reg[i]);
		}
		for (int i = 0; i < strlen(sval); i++) {
			sval[i] = toupper(sval[i]);
		}
		
		int val = 0;
		if (sval[1] == 'D') {
			sscanf(sval+2, "%d", &val);
		}
		else {
			sscanf(sval, "%02X", &val);
		}
		if (verb)
			printf("%s -> %02X [%d]\n", reg, val, val);
		
		if (strcmp("X", reg) == 0) 
			cpu.X = val;
		else if (strcmp("Y", reg) == 0)
			cpu.Y = val;
		else if (strcmp("A", reg) == 0)
			cpu.A = val;
		else if (strcmp("SP", reg) == 0)
			cpu.SP = val;
		else if (strcmp("PC", reg) == 0)
			cpu.PC = val;
		else if (strcmp("STAT", reg) == 0)
			cpu.stat = val;
		else {
			printf("Invalid Register: %s", reg);
		}
	}
	else if (strcmp("DUMPMEM", command) == 0) {
		char saddr[64] = {0};
		int len = 16;
		unsigned short addr = 0;
		
		sscanf(input, "%s%s%d", command, saddr, &len);
		for (int i = 0; i < strlen(saddr); ++i) {
			saddr[i] = toupper(saddr[i]);
		}
		
		sscanf(saddr, "%04X", &addr);
		
		if (verb) {
			printf("Dumping [%d] bytes of memory from %04X\n", len, addr);
		}
		dumpmem(memory, addr, len);
		printf("\n");
	}
	else if (strcmp("SETMEM", command) == 0) {
		char saddr[64] = {0};
		char sval[64] = {0};
		unsigned short addr = 0;
		
		sscanf(input, "%s%s%s", command, saddr, sval);
		for (int i = 0; i < strlen(saddr); ++i) {
			saddr[i] = toupper(saddr[i]);
		}
		printf("saddr: %s\nsval: %s\n", saddr, sval);
		
		sscanf(saddr, "%04X", &addr);
		
		for (int i = 0; i < strlen(sval); i++) {
			sval[i] = toupper(sval[i]);
		}
		
		unsigned char val = 0;
		
		if (sval[1] == 'D') {
			sscanf(sval+2, "%d", &val);
		}
		else {
			sscanf(sval, "%02X", &val);
		}
		
		memory.write(addr, val);

		if (verb) {
			printf("Writing %02X to %04X\n", memory.read(addr), addr);
		}
	
	}
	else if (strcmp("EXIT", command) == 0) {
		exit(0);
	}
	else if (strcmp("RUN", command) == 0) {
		return -1;
	}
	else {
		printf("Invalid Command %s;\nrun HELP for help\n", command);
	}
	
	

	return 0;
}

int main(int argc, char **argv) {
	//PROCESS ARGS
	bool inter = false;
	bool help = false;
	bool verb = false;
	FILE *logfile = NULL;
	FILE *file = NULL;
	int start = 0;

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			switch(argv[i][1]) {
				case 'i':
					inter = true;
					break;
				case '?':
				case 'h':
					help = true;
					break;
				case 'l':
					i++;
					if (i >= argc) {
						break;
					}
					logfile = fopen(argv[i], "wb");
					break;
				case 'v':
					verb = true;
					break;
				case 's':
					i++;
					if (i >= argc) {
						break;
					}
					
					if (strncmp("0x", argv[i], 2))
						start = strtol(argv[i], NULL, 10);
					else
						start = strtol(argv[i]+2, NULL, 16);
					break;
				default:
					break;
			}
		}
		else {
			file = fopen(argv[i], "rb"); 
			if (file == NULL) {
				printf("File %s not found\n", argv[i]);	
			}
		}
	}
	
	if (help) {
		printf("USAGE: %s [OPTIONS] [FILE]\n\nOPTIONS:\n-? | -h\tHELP, print this help\n-i\tRun with Interactive Prompt\n-v\tRun Verbosely\n-s\tSpecify start location, copied to FFFC/FD in memory before execution\n\nFILE\tBinary File copied to 0000 in memory before execution\n\n", argv[0]);
		return -1;
	}

	memory memory;
	cpu cpu(memory);

	if (file != NULL)
		loadprog(memory, file, verb);

	if (start) {
		if (verb)
			printf("Writing start location [%04X] to memory\n", start);
		memory.write(0xFFFC, start >> 8);
		memory.write(0xFFFD, start &  0xFF);
		if (verb)
			printf("\tWritten %02X to FFFC\n\tWritten %02X to FFFD\n\n", memory.read(0xFFFC), memory.read(0xFFFD));
	}

	cpu.reset();
	int nsteps = 0;
	bool clock = true;
	while (clock){
		nsteps--;
		
		while (inter && nsteps <= 0) {
			nsteps = prompt(cpu, memory, verb);
			if (nsteps == -1) {
				inter = false;
			}
		}
		
		clock = cpu.clock();

		if (verb) {
			//PRINT CPU STATE
			printf("Cycles: %d; PC: %04X; A: %02X; X: %02X; Y: %02X; SP: %02X; STAT: ", cpu.clocks, cpu.PC, cpu.A, cpu.X, cpu.Y, cpu.SP);
			std::bitset<8> stat(cpu.stat);
			std::cout << stat << '\n';
		}	

	}

	//EXIT
	if (file != NULL)
		fclose(file);
	if (logfile != NULL)
		fclose(logfile);
	return 0;
}
