#include "online_lossless.h"

online_lossless::online_lossless(string& in_file,string& out_file,bool write):
	ins(in_file),outs(out_file){
	this->write=write;
}

//HEADER for compressed
//Magic number (CAVN+)
//Stereo bool
//Predictor order
//Number of samples
//Sample Frequency				(NOT USED NOW)
//Initial m
//Window size
//M calc int
//Initial m Y                    (IF STEREO)
//Window size Y                  (IF STEREO)
//M calc int Y                   (IF STEREO)
//First sample in binary code
//Rest of samples in golomb

uint online_lossless::encode(){

	wav wv(ins);
	wv.load();

    assert(wv.getNumOfChannels() < 3);
    bool stereo = (wv.getNumOfChannels()==2);

	bitstream bss(outs.c_str(),std::ios::trunc|std::ios::binary|std::ios::out);
	bitstream_wrapper bs(bss,write);

	golomb_bitstream gb(initial_m,window_size,m_calc_int,bs);
	//golomb_bitstream gb(initial_m,bs);
	predictor pd(false,pred_order);

	//Write magic
	bs.writeNChars((char*) magic,strlen(magic));

	//Write stereo flag
	bs.writeBit(stereo);

    //Write predictor order
    bs.writeNBits(pred_order,2);

	//Write num of samples
	bs.writeNBits(wv.getNumSamples(),sizeof(uint32_t)*8);
	
	//Write initial m
	bs.writeNBits(initial_m,sizeof(uint)*8);
	
	//Write window size
	bs.writeNBits(window_size,sizeof(uint)*8);
	
	//Write M calculation interval
	bs.writeNBits(m_calc_int,sizeof(uint)*8);

	if (!stereo){

        //Get samples from wav
        std::vector<short> smp = wv.getChannelData(1);
        auto it=smp.begin();

        //Sending first sample in natural binary
        //Because the predictor cant predict a good first value
        bs.writeNBits((uint32_t) pd.residual(*it),sizeof(short)*8+1);

        for(it+=1;it<smp.end();it++)
            gb.write_signed_val(pd.residual(*it));

    } else{
	    predictor pd_y(false,pred_order);

        //Write initial m Y
        bs.writeNBits(y_initial_m,sizeof(uint)*8);

        //Write window size Y
        bs.writeNBits(y_window_size,sizeof(uint)*8);

        //Write M calculation interval Y
        bs.writeNBits(y_m_calc_int,sizeof(uint)*8);

        golomb_bitstream gb_y(y_initial_m,y_window_size,y_m_calc_int,bs);

        short samp1 = wv.get(0,1), samp2 = wv.get(0,2);

        bs.writeNBits((uint32_t) pd.residual(((int)samp1 + samp2)/2), sizeof(short)*8+1);
        //gb_y.write_signed_val((int)samp1 - samp2);
        bs.writeNBits((uint32_t) pd_y.residual((int)samp1 - samp2), sizeof(short)*8+1);

        for(uint i = 1; i < wv.getNumSamples(); i++){
            samp1 = wv.get(i,1);
            samp2 = wv.get(i,2);
            gb.write_signed_val(pd.residual(( (int) samp1 + samp2 ) / 2));
            gb_y.write_signed_val(pd_y.residual((int) samp1 - samp2));
        }
	}
	return bs.getBits();
}

int online_lossless::decode(){
	wav wv(outs);
	bitstream bss(ins.c_str(),std::ios::binary|std::ios::in);
	bitstream_wrapper bs(bss,true);

	//Read header

	//Check magic
	char is_magic[strlen(magic)];
	bs.readNChars(is_magic,sizeof(is_magic)); 
	if(strncmp(magic,is_magic,strlen(magic))!=0) return -1;

	//Read stereo flag
	bool stereo = bs.readBit();

    //Read predictor order
    pred_order=bs.readNBits(2);

	//Read predictor order
	predictor pd(false,pred_order);

	//Read num of samples
	uint num_samp=bs.readNBits(sizeof(uint32_t)*8);
	
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
	wv.setNumOfChannels(stereo?2:1);
	wv.setNumSamples(num_samp);
	wv.setSamplesPerSec(44100);
	wv.createStructure();

    uint smp_ptr=0;
    if (!stereo){
        //Read first sample (in natural binary)
        short sample=pd.reconstruct(bs.readNBits(sizeof(short)*8+1));
        wv.insert(smp_ptr++,1,sample);

        //Reconstruct the rest of the samples
        for(uint i=1;i<num_samp;i++)
            wv.insert(smp_ptr++,1,pd.reconstruct(gb.read_signed_val()));


    }else{
        predictor pd_y(false,pred_order);

        //Read initial m
        y_initial_m=bs.readNBits(sizeof(uint)*8);

        //Read window size
        y_window_size=bs.readNBits(sizeof(uint)*8);

        //Read M calculation interval
        y_m_calc_int=bs.readNBits(sizeof(uint)*8);

        golomb_bitstream gb_y(y_initial_m, y_window_size,y_m_calc_int,bs);


        //Read first x (in natural binary) and y values
        short x = pd.reconstruct(bs.readNBits(sizeof(short)*8+1));
        //short y = gb_y.read_signed_val();
        short y = pd_y.reconstruct(bs.readNBits(sizeof(short)*8+1));

        //reconstruct channels' sample's value.
        wv.insert(smp_ptr, 1, (((int) 2*x + (x>0?0:-1)*(std::abs(y)%2) + y) / 2 ));
        wv.insert(smp_ptr++, 2, (((int) 2*x + (x>0?0:-1)*(std::abs(y)%2) - y) / 2 ));

        while(smp_ptr<num_samp){
            x = pd.reconstruct(gb.read_signed_val());
            //y = gb_y.read_signed_val();
            y = pd_y.reconstruct(gb_y.read_signed_val());

            wv.insert(smp_ptr, 1, (((int) 2*x + (x>0?1:-1)*(std::abs(y)%2) + y) / 2 ));
            wv.insert(smp_ptr++, 2, (((int) 2*x + (x>0?1:-1)*(std::abs(y)%2) - y) / 2 ));
        }
    }

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

void online_lossless::set_y_window_size(uint ws){
    y_window_size = ws;
}

void online_lossless::set_y_m_calc_int(uint mci){
    y_m_calc_int = mci;
}

void online_lossless::set_y_initial_m(uint m){
    y_initial_m = m;
}

void online_lossless::set_pred_order(uint order){
    pred_order = order;
}
