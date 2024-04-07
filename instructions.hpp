struct instruction {
	char SYM[4];
	void *execute();
	void *addr();
};

