#include "bitstream.h"
using namespace std;

void bitstream::writeBit(uint8_t val) {
	byte |= (val & 0x01) << pointer;
	if (pointer > 0) {
		pointer--;                    
		return;       
	}                    
	write((char*)&byte, 1);
	pointer = 7;
	byte = 0;
}

void bitstream::writeNBits(uint32_t val, uint n) {
	uint bit;
	for (int i = n-1; i >= 0; i--) {
        bit = (val >> i%32) & 0x01;
    	writeBit(bit);
	}
}

void bitstream::writeNBits(uint32_t* val, uint n) {
	for (uint i = 0; i < n/32.0; i++) {
		std::cout << "aa";
		std::cout << val[i] << std::endl;
		if (i == n/32)
			writeNBits(val[i], n%32);
		else
			writeNBits(val[i], 32);
	}
}

uint8_t bitstream::readBit() {
	uint8_t bit;
	if (pointer < 0) {
		read((char*)&byte, 1); 
		pointer = 7;
	}
	bit = ((byte >> pointer) & 0x01);
	pointer--;
	return bit;
}

void bitstream::readBit(uint8_t* bit) {
	if (pointer < 0) {
		read((char*)&byte, 1); 
		pointer = 7;
	}
	*bit = ((byte >> pointer) & 0x01);
	pointer--;
}

uint32_t bitstream::readNBits(uint n) {
	assert(n <= 32);
	uint32_t value = 0;
	for (uint i = 0; i < n; i++) {
		value = value << 1 | readBit();
	}
	return value;
}

void bitstream::readNBits(uint32_t* bits, uint n) {
	uint32_t ref;
	for (uint i = 0; i < n/32.0; i++) {
		if (i == n/32)
			bits[i] = readNBits(n%32);
		else
			bits[i] = readNBits(32);
	}
}

void bitstream::writeChar(char val) {
	writeNBits(val, sizeof(char)*8);
}

void bitstream::writeNChars(char* val, uint n) {
	for (uint i = 0; i < n; i++) {
		writeNBits(*(val+i), sizeof(char)*8);
	}
}

char bitstream::readChar() {
	return readNBits(sizeof(char)*8);
}

void bitstream::readChar(char* c) {
	*c = readNBits(sizeof(char)*8);
}

void bitstream::readNChars(char* c, uint n) {
	for (uint i = 0; i < n; i++) {
		*(c+i) = readNBits(sizeof(char)*8);
	}
}

void bitstream::addPadding() {
	for (int i = 0; i < 7 - pointer; i++) {
		writeBit(0);
	}
}

bitstream::~bitstream() {
	if (writeMode)
		addPadding();
	close();
}
