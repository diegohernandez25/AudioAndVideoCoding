#include "golomb_bitstream.h"

//TODO code first value without golomb
//TODO cena de se o g nao for uma potencia poupar espaco

golomb_bitstream::golomb_bitstream(uint m,bitstream& bt) : gl(m),bt(bt){}

golomb_bitstream::golomb_bitstream(uint initial_m,uint blk_size,uint calc_interval,bitstream& bt) : gl(initial_m,blk_size,calc_interval),bt(bt) {}

void golomb_bitstream::write_signed_val(int val){
	uint sizeof_r=std::ceil(std::log2(gl.get_m()));

	std::tuple<uint,uint> enc = gl.signed_encode(val);
	uint q=std::get<0>(enc); //represented in unary (comma) code
    uint r=std::get<1>(enc); //represented in (natural) binary code

	//Write q
	//0 is the seperator of the unary code
	for(int i=0;i<q;i++) bt.writeBit(1);
	bt.writeBit(0);

	//Write r
	bt.writeNBits(&r,sizeof_r);

}

void golomb_bitstream::write_val(uint val){
	uint sizeof_r=std::ceil(std::log2(gl.get_m()));

	std::tuple<uint,uint> enc = gl.encode(val);
	uint q=std::get<0>(enc); //represented in unary (comma) code
    uint r=std::get<1>(enc); //represented in (natural) binary code

	//Write q
	//0 is the seperator of the unary code
	for(int i=0;i<q;i++) bt.writeBit(1);
	bt.writeBit(0);

	//Write r
	bt.writeNBits(&r,sizeof_r);
}

int golomb_bitstream::read_signed_val(){
	uint sizeof_r=std::ceil(std::log2(gl.get_m()));
	uint q=0;
	uint r=0;

	//Read q
	while(bt.readBit()==1) q++;
	
	//Read r
	bt.readNBits(&r,sizeof_r);
	return gl.signed_decode(q,r);
}

uint golomb_bitstream::read_val(){
	uint sizeof_r=std::ceil(std::log2(gl.get_m()));
	uint q=0;
	uint r=0;

	//Read q
	while(bt.readBit()==1) q++;
	
	//Read r
	bt.readNBits(&r,sizeof_r);
	return gl.decode(q,r);
}
