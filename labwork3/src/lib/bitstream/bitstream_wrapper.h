#ifndef H_BITSTREAM_WRAPPER
#define H_BITSTREAM_WRAPPER

#include <iostream>
#include <fstream>
#include <bitset>
#include <cassert>
#include "bitstream.h"

using std::fstream;

class bitstream_wrapper{
    public:
        bitstream_wrapper(bitstream& bs,bool write) : bs(bs) {
			acc=0;
			this->write=write;
        };
        void writeBit(uint8_t val);
        void writeNBits(uint32_t val, uint n);
        void writeNBits(uint32_t* val, uint n);
        void writeChar(char val);
        void writeNChars(char* val, uint n);
        void addPadding();

        uint8_t readBit();
        void readBit(uint8_t* bit);
        uint32_t readNBits(uint n);
        void readNBits(uint32_t* bits, uint n);
        char readChar();
        void readChar(char* c);
        void readNChars(char* c, uint n);

		uint getBits();
        
    private:
		bool write;
		uint acc;
		bitstream& bs;
};

#endif
