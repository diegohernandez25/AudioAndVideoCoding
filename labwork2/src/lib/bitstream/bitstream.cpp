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
	for (uint i = 0; i < n; i++) {
        bit = (val >> i%32) & 0x01;
    	writeBit(bit);
	}
}

void bitstream::writeNBits(uint32_t* val, uint n) {
	uint bit;
	for (uint i = 0; i < n; i++) {
        bit = ((*(val + i/32)) >> i%32) & 0x01;
    	writeBit(bit);
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
	for (int i = 0; i < n; i++) {
		value = value << 1 | readBit();
	}
	return value;
}

void bitstream::readNBits(uint32_t* bits, uint n) {
	uint32_t value = 0;
	uint32_t ref;
	for (uint i = 0; i < n; i++) {
		ref = (*(bits + i/32));
		ref = ref | readBit();
	}
}

bitstream::~bitstream() {
	// TODO padding
	close();
}