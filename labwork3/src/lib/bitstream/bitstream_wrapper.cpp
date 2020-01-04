#include "bitstream_wrapper.h"
using namespace std;

void bitstream_wrapper::writeBit(uint8_t val) {
	acc++;	
	if(write) bs.writeBit(val);

}

void bitstream_wrapper::writeNBits(uint32_t val, uint n) {
	acc+=n;	
	if(write) bs.writeNBits(val,n);
}

void bitstream_wrapper::writeNBits(uint32_t* val, uint n) {
	acc+=n;	
	if(write) bs.writeNBits(val,n);
}

void bitstream_wrapper::writeChar(char val) {
	acc+=sizeof(char)*8;
	if(write) bs.writeChar(val);
}

void bitstream_wrapper::writeNChars(char* val, uint n) {
	acc+=sizeof(char)*8*n;
	if(write) bs.writeNChars(val,n);
}

void bitstream_wrapper::addPadding() {
	if(acc%8!=0) acc+=8-acc%8;
	if(write) bs.addPadding();
}

uint8_t bitstream_wrapper::readBit(){
	acc++;
	return bs.readBit();
}
void bitstream_wrapper::readBit(uint8_t* bit){
	acc++;
	bs.readBit(bit);
}
uint32_t bitstream_wrapper::readNBits(uint n){
	acc+=n;
	return bs.readNBits(n);
}
void bitstream_wrapper::readNBits(uint32_t* bits, uint n){
	acc+=n;
	bs.readNBits(bits,n);
}
char bitstream_wrapper::readChar(){
	acc+=sizeof(char)*8;
	return bs.readChar();
}
void bitstream_wrapper::readChar(char* c){
	acc+=sizeof(char)*8;
	bs.readChar(c);
}
void bitstream_wrapper::readNChars(char* c, uint n){
	acc+=sizeof(char)*8*n;
	bs.readNChars(c,n);
}

uint bitstream_wrapper::getBits(){ return acc; }

