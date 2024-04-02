class bus {
	public:
		unsigned char read(unsigned short addr);
		void write(unsigned short addr, unsigned char data);
		bus() {
			memory;
		};
	private:
		unsigned char memory[65536]; //TEMP, MAKE MORE ACCURATE LATER
};
