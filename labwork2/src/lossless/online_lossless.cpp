#include "online_lossless.h"

online_lossless::online_lossless(string& in_file,string& out_file):
	ins(in_file),outs(out_file){}

//HEADER for compressed
//Magic number (CAVN+)
//Number of channels			(NOT USED NOW)
//Number of samples
//Sample Frequency				(NOT USED NOW)
//Initial m
//Window size
//M calc int
//First sample in binary code 	
//Rest of samples in golomb

void online_lossless::encode(){

	wav wv(ins);
	wv.load();

	bitstream bs(outs.c_str(),std::ios::trunc|std::ios::binary|std::ios::out);
	golomb_bitstream gb(initial_m,window_size,m_calc_int,bs);
	//golomb_bitstream gb(initial_m,bs);
	predictor pd(false);

	//Write magic
	bs.writeNChars((char*) magic,strlen(magic));

	//Write num of samples
	bs.writeNBits(wv.getNumSamples(),sizeof(uint32_t)*8);
	
	//Write initial m
	bs.writeNBits(initial_m,sizeof(uint)*8);
	
	//Write window size
	bs.writeNBits(window_size,sizeof(uint)*8);
	
	//Write M calculation interval
	bs.writeNBits(m_calc_int,sizeof(uint)*8);

	//Get samples from wav
	std::vector<short> smp = wv.getChannelData(1);
	auto it=smp.begin();

	//Sending first sample in natural binary 
	//Because the predictor cant predict a good first value 
	bs.writeNBits((uint32_t) pd.residual(*it),sizeof(short)*8+1); 

	for(it+=1;it<smp.end();it++)
		gb.write_signed_val(pd.residual(*it));
}

int online_lossless::decode(){
	wav wv(outs);
	bitstream bs(ins.c_str(),std::ios::binary|std::ios::in);
	predictor pd(false);

	//Read header

	//Check magic
	char is_magic[strlen(magic)];
	bs.readNChars(is_magic,sizeof(is_magic)); 
	if(strncmp(magic,is_magic,strlen(magic))!=0) return -1;

	//Read num of samples
	int num_samp=bs.readNBits(sizeof(uint32_t)*8);
	
	//Read initial m
	initial_m=bs.readNBits(sizeof(uint)*8);
	
	//Read window size
	window_size=bs.readNBits(sizeof(uint)*8);
	
	//Read M calculation interval
	m_calc_int=bs.readNBits(sizeof(uint)*8);

	//Init golomb module with param given
	golomb_bitstream gb(initial_m,window_size,m_calc_int,bs);
	//golomb_bitstream gb(initial_m,bs);

	//End the creation of the header
	wv.setNumOfChannels(1);
	wv.setNumSamples(num_samp);
	wv.setSamplesPerSec(44100);
	wv.createStructure();

	uint smp_ptr=0;
	//Read first sample (in natural binary)
	short sample=pd.reconstruct(bs.readNBits(sizeof(short)*8+1));
	wv.insert(smp_ptr++,1,sample);

	//Reconstruct the rest of the samples
	for(int i=1;i<num_samp;i++)
		wv.insert(smp_ptr++,1,pd.reconstruct(gb.read_signed_val()));

	//Write to file
	wv.dump();	

	return 0;
}

void online_lossless::set_window_size(uint ws){
    window_size=ws;
}

void online_lossless::set_m_calc_int(uint mci){
    m_calc_int=mci;
}

void online_lossless::set_initial_m(uint m){
    initial_m=m;
}
