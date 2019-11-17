#ifndef H_BITSTREAM
#define H_BITSTREAM

#include <iostream>
#include <fstream>
#include <bitset>

using std::fstream;

class bitstream : public fstream {
    public:
        bitstream(const char *fname, ios_base::openmode mode) : fstream(fname, mode) {  };
        ~bitstream();
        int writeBit(uint8_t val);
        uint8_t readBit();
        // uint8_t readBit(uint32_t* val);
        uint8_t writeNBits(uint32_t* val, int n);
        uint8_t writeNBits(uint32_t val, int n);
       // uint32_t readNBits(uint32_t* val, uint n);
        uint32_t readNBits(uint n);
    private:
        uint8_t byte = 0;
        int pointer = 7;
        int byteCount;
};

#endif
