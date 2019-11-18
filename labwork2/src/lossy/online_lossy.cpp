#include "online_lossy.h"

online_lossy::online_lossy(string& in_file,string& out_file):
	ins(in_file),outs(out_file){}

//HEADER for compressed
//Magic number (CAVN+)
//Stereo bool
//Number of samples
//Sample Frequency				(NOT USED NOW)
//N Quant
//Initial m
//Window size
//M calc int
//Initial m Y                    (IF STEREO)
//Window size Y                  (IF STEREO)
//M calc int Y                   (IF STEREO)
//First sample in binary code 	
//Rest of samples in golomb

void online_lossy::encode() {

    wav wv(ins);
    wv.load();

    assert(wv.getNumOfChannels() < 3);
    bool stereo = (wv.getNumOfChannels()==2);

    bitstream bs(outs.c_str(), std::ios::trunc | std::ios::binary | std::ios::out);
    golomb_bitstream gb(initial_m, window_size, m_calc_int, bs);
    //golomb_bitstream gb(initial_m,bs);
    predictor pd(true);
    quant qt;
    int quant_resid;

    //Write magic
    bs.writeNChars((char *) magic, strlen(magic));

    //Write stereo flag
    bs.writeBit(stereo);

    //Write num of samples
    bs.writeNBits(wv.getNumSamples(), sizeof(uint32_t) * 8);

    //Write N quant
    bs.writeNBits(nr_quant, sizeof(uint) * 8);

    //Write initial m
    bs.writeNBits(initial_m, sizeof(uint) * 8);

    //Write window size
    bs.writeNBits(window_size, sizeof(uint) * 8);

    //Write M calculation interval
    bs.writeNBits(m_calc_int, sizeof(uint) * 8);

    //MEDIA -> PREDITOR -> QUANTIZAÇÃO

    if(!stereo){
        //Get samples from wav
        std::vector<short> smp = wv.getChannelData(1);
        auto it = smp.begin();

        //Sending first sample in natural binary
        //Because the predictor cant predict a good first value
        quant_resid = qt.midrise(nr_quant, pd.residual(*it));
        pd.updateBufferQuant(quant_resid);
        bs.writeNBits(quant_resid >> nr_quant, sizeof(short) * 8 + 1 - nr_quant);

        for (it += 1; it < smp.end(); it++) {
            quant_resid = qt.midrise(nr_quant, pd.residual(*it));
            pd.updateBufferQuant(quant_resid);
            gb.write_signed_val(quant_resid >> nr_quant);

        }
    } else {
        predictor pd_y(true);

        //Write initial m Y
        bs.writeNBits(y_initial_m,sizeof(uint)*8);

        //Write window size Y
        bs.writeNBits(y_window_size,sizeof(uint)*8);

        //Write M calculation interval Y
        bs.writeNBits(y_m_calc_int,sizeof(uint)*8);

        golomb_bitstream gb_y(y_initial_m,y_window_size,y_m_calc_int,bs);

        short samp1 = wv.get(0,1), samp2 = wv.get(0,2);

        int quant_resid_x = qt.midrise(nr_quant, pd.residual(((int)samp1 + samp2)/2));
        int quant_resid_y = qt.midrise(nr_quant, pd_y.residual((int)samp1 - samp2));

        pd.updateBufferQuant(quant_resid_x);
        pd_y.updateBufferQuant(quant_resid_y);

        bs.writeNBits(quant_resid_x >> nr_quant, sizeof(short) * 8 + 1 - nr_quant);
        bs.writeNBits(quant_resid_y >> nr_quant, sizeof(short) * 8 + 1 - nr_quant);

        for(uint i = 1; i < wv.getNumSamples(); i++){
            samp1 = wv.get(i,1);
            samp2 = wv.get(i,2);

            quant_resid_x = qt.midrise(nr_quant, pd.residual(((int)samp1 + samp2)/2));
            quant_resid_y = qt.midrise(nr_quant, pd_y.residual((int)samp1 - samp2));

            pd.updateBufferQuant(quant_resid_x);
            pd_y.updateBufferQuant(quant_resid_y);

            gb.write_signed_val(quant_resid_x >> nr_quant);
            gb_y.write_signed_val(quant_resid_y >> nr_quant);
            //cout<<i<< endl;
        }
    }

}

int online_lossy::decode(){
	wav wv(outs);
	bitstream bs(ins.c_str(),std::ios::binary|std::ios::in);
	predictor pd(false);

	//Read header

	//Check magic
	char is_magic[strlen(magic)];
	bs.readNChars(is_magic,sizeof(is_magic)); 
	if(strncmp(magic,is_magic,strlen(magic))!=0) return -1;

    //Read stereo flag
    bool stereo = bs.readBit();

	//Read num of samples
	uint num_samp=bs.readNBits(sizeof(uint32_t)*8);

    //Read N quant
    nr_quant=bs.readNBits(sizeof(uint) * 8);

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
        short sample=pd.reconstruct(bs.readNBits(sizeof(short)*8+1-nr_quant)<<nr_quant);
        wv.insert(smp_ptr++,1,sample);

        //Reconstruct the rest of the samples
        for(uint i=1;i<num_samp;i++)
            wv.insert(smp_ptr++,1,pd.reconstruct(gb.read_signed_val()<<nr_quant));

    }else{
        predictor pd_y(false);

        //Read initial m
        y_initial_m=bs.readNBits(sizeof(uint)*8);

        //Read window size
        y_window_size=bs.readNBits(sizeof(uint)*8);

        //Read M calculation interval
        y_m_calc_int=bs.readNBits(sizeof(uint)*8);

        golomb_bitstream gb_y(y_initial_m, y_window_size,y_m_calc_int,bs);


        short x = pd.reconstruct(bs.readNBits(sizeof(short)*8+1-nr_quant)<<nr_quant);
        short y = pd_y.reconstruct(bs.readNBits(sizeof(short)*8+1-nr_quant)<<nr_quant);
        uint8_t rem = (y%2)?1:0;

        //resonstruct channels' sample's value.
        wv.insert(smp_ptr, 1, (((int) 2*x + rem + y) / 2 ));
        wv.insert(smp_ptr++, 2, (((int) 2*x + rem - y) / 2 ));

        while(smp_ptr<num_samp){
            x = pd.reconstruct(gb.read_signed_val()<<nr_quant);
            y = pd_y.reconstruct(gb_y.read_signed_val()<<nr_quant);

            rem = (y%2)?1:0;

            wv.insert(smp_ptr, 1, (((int) 2*x + rem + y) / 2 ));
            wv.insert(smp_ptr++, 2, (((int) 2*x + rem - y) / 2 ));
        }
    }
	//Write to file
	wv.dump();
	return 0;
}

void online_lossy::set_window_size(uint ws){
    window_size=ws;
}

void online_lossy::set_m_calc_int(uint mci){
    m_calc_int=mci;
}

void online_lossy::set_initial_m(uint m) {
    initial_m = m;
}

void online_lossy::set_nr_quant(uint nq){
    nr_quant=nq;
}

void online_lossy::set_y_window_size(uint ws){
    y_window_size=ws;
}

void online_lossy::set_y_m_calc_int(uint mci){
    y_m_calc_int = mci;
}

void online_lossy::set_y_initial_m(uint m){
    y_initial_m = m;
}
