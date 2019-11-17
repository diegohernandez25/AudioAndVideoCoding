#include "bitstream.h"
using namespace std;

int bitstream::writeBit(uint8_t val) {
	byte |= (val & 0x01) << pointer;
	if (pointer > 0) {
		pointer--;                    
		return 0;                                   
	}                    
	write((char*)&byte, 1);
	pointer = 7;
	byte = 0;
	return 0;
}

uint8_t bitstream::writeNBits(uint32_t* val, int n){
  uint bit;
  for (uint i = 0; i < n; i++) {
        bit = ((*(val + i/32)) >> i%32) & 0x1;
    	writeBit(bit);
	}
	return 0;
}

uint8_t bitstream::writeNBits(uint32_t val, int n){
  uint bit;
  for (uint i = 0; i < n; i++) {
        bit = (val >> i%32) & 0x1;
    	writeBit(bit);
	}
	return 0;
}

uint8_t bitstream::readBit() {
	uint8_t bit;
	if (pointer == 7) {
		read((char*)&byte, 1); 
	}
	bit = ((byte >> pointer) & 0x01);
	pointer--;
	if (pointer < 0)
		pointer = 7;
	return bit;
}

uint32_t bitstream::readNBits(uint n){
	uint32_t value = 0;
	for (int i = 0; i < n; i++){
		value = value << 1 | readBit();
	}
	return value;
}

bitstream::~bitstream() {
	close();
}