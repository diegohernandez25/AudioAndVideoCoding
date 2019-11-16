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
        int writeNBits(uint* val, uint nBits);
        int readNBits(uint* val,uint nBits);
        int grantWrite();
    private:
        uint8_t byte = 0;
        int pointer = 7;
        int byteCount;
};
