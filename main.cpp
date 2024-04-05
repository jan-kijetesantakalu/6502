#include "memory.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

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
		}
	}

	fclose(file);
	fclose(logfile);

}
