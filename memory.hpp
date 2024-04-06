class memory {
	public:
		unsigned char read(unsigned short addr);
		void write(unsigned short addr, unsigned char v);
	private:
		unsigned char data[65536] = {0};
};
