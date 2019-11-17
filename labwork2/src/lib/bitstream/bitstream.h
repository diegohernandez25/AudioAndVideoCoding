#ifndef H_BITSTREAM
#define H_BITSTREAM

#include <iostream>
#include <fstream>
#include <bitset>

// TODO chars

using std::fstream;

class bitstream : public fstream {
    public:
        bitstream(const char *fname, ios_base::openmode mode) : fstream(fname, mode) {
            if (mode & std::ios::out)
                pointer = 7;
            else if (mode & std::ios::in)
                pointer = -1;
        };
        ~bitstream();
        void writeBit(uint8_t val);
        void writeNBits(uint32_t val, uint n);
        void writeNBits(uint32_t* val, uint n);
        uint8_t readBit();
        void readBit(uint8_t* bit);
        uint32_t readNBits(uint n);
        void readNBits(uint32_t* bits, uint n);

    private:
        uint8_t byte = 0;
        int pointer;
        int byteCount;
};

#endif
