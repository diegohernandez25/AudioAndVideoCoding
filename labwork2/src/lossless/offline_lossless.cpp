#include "offline_lossless.h"

offline_lossless::offline_lossless(string& in_file,string& out_file):
	ins(in_file),outs(out_file){}

//HEADER for compressed
//Magic number (CAVF+)
//Number of channels			(NOT USED NOW)
//Number of samples				
//Sample Frequency				(NOT USED NOW)
//Window size					
//M Calc Int				
//M list	
//First sample in binary code 	
//Rest of samples in golomb

void offline_lossless::encode(){

	wav wv(ins);
	wv.load();
	bitstream bss(outs.c_str(),std::ios::trunc|std::ios::binary|std::ios::out);
	bitstream_wrapper bs(bss,true);
	predictor pd(false,pred_order);

	uint m_list_size=((wv.getNumSamples()-1)-window_size+1)/(m_calc_int+1); 
	std::vector<uint> m_list((std::vector<uint>::size_type) m_list_size);
	std::vector<uint16_t> residual_list((std::vector<int>::size_type) wv.getNumSamples()-1);
	
	//First run golomb to find ideal M for each window
	//And store residuals
	golomb g_m(1,window_size,m_calc_int); //initial_m not relevant since we are getting the m inside
	std::vector<int16_t> smp = wv.getChannelData(1);
	auto it=smp.begin();
	pd.residual(*it);
	
	uint res_ptr=0;
	short res;
	bool window_full=false;
	uint window_cnt=0; 	
	uint m_pointer=0; 
	for(it+=1;it<smp.end();it++){
		res=pd.residual(*it);
		g_m.signed_encode(res);
		residual_list[res_ptr++]=res;	

		if((!window_full&&window_cnt==window_size-1)||
			(window_full&&window_cnt==m_calc_int)){
			window_full=true;
			m_list[m_pointer++]=g_m.get_m();
			window_cnt=0;
			cout<<g_m.get_m()<<endl;
 		}
		else
			window_cnt++;
	}
		
	//------------------------------
	//Now run golomb on the residuals with the new M
	//And write to files
	bs.writeNChars((char*) magic,strlen(magic));

	//Write preditor order
	bs.writeNBits(pred_order,2);
	
	//Write the nr of samples
	bs.writeNBits(wv.getNumSamples(),sizeof(uint32_t)*8);

	//Write the window size
	bs.writeNBits(window_size,sizeof(uint32_t)*8);
	
	//Write the m calc interval
	bs.writeNBits(m_calc_int,sizeof(uint32_t)*8);

	//Write the list of m
	for(uint& tm : m_list)	
		bs.writeNBits((uint32_t) tm,sizeof(short)*8); 


	//Sending first sample in natural binary 
	//Because the predictor cant predict a good first value 
	bs.writeNBits((uint32_t) smp[0] ,sizeof(short)*8); 


	m_pointer=0;
	window_cnt=0;
	window_full=false;
	golomb_bitstream gb(m_list[m_pointer++],bs);
	for(uint16_t& residual : residual_list){
		gb.write_signed_val((uint) residual);

		if((!window_full&&window_cnt==window_size-1)||
			(window_full&&window_cnt==m_calc_int)){
			window_full=true;
			gb.set_m(m_list[m_pointer++]);
			window_cnt=0;
		}
		else
			window_cnt++;

	}

}

int offline_lossless::decode(){
	wav wv(outs);
	bitstream bss(ins.c_str(),std::ios::binary|std::ios::in);
	bitstream_wrapper bs(bss,true);

	//Check magic
    char is_magic[strlen(magic)];
    bs.readNChars(is_magic,sizeof(is_magic)); 
    if(strncmp(magic,is_magic,strlen(magic))!=0) return -1;

	//Read predictor order
    pred_order=bs.readNBits(2);
	predictor pd(false,pred_order);

	//Read the nr of samples
	uint num_samp=(uint) bs.readNBits(sizeof(uint32_t)*8);

	//Read the window size
	window_size=bs.readNBits(sizeof(uint32_t)*8);
	
	//Read the m calc interval
	m_calc_int=bs.readNBits(sizeof(uint32_t)*8);

    //End the creation of the header
    wv.setNumOfChannels(1);
    wv.setNumSamples(num_samp);
    wv.setSamplesPerSec(44100);
    wv.createStructure();

	//Init list of m
	uint m_list_size=((num_samp-1)-window_size+1)/(m_calc_int+1);
    std::vector<uint> m_list((std::vector<uint>::size_type) m_list_size);

	//Read the list of m
	for(uint& tm : m_list)	
		tm=bs.readNBits(sizeof(short)*8); 

	//Read first sample (in natural binary)
	uint smp_ptr=0;
	short sample=pd.reconstruct(bs.readNBits(sizeof(short)*8));
	wv.insert(smp_ptr++,1,sample);

    bool window_full=false;
    uint window_cnt=0;
    uint m_pointer=0;
	golomb_bitstream gb(m_list[m_pointer++],bs);

    for(uint i=1;i<num_samp;i++){
        wv.insert(smp_ptr++,1,pd.reconstruct(gb.read_signed_val()));

        if((!window_full&&window_cnt==window_size-1)||
            (window_full&&window_cnt==m_calc_int)){
            window_full=true;
            gb.set_m(m_list[m_pointer++]);
            window_cnt=0;
        }
        else
            window_cnt++;

    }
	wv.dump();

	return 0;
}

void offline_lossless::set_window_size(uint ws){
	window_size=ws;
}

void offline_lossless::set_m_calc_int(uint mci){
	m_calc_int=mci;
}

void offline_lossless::set_pred_order(uint order){
	pred_order=order;
}
