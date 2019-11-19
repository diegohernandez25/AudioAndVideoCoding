#include "wav_utils.h"


double wav_utils::get_snr(string orig,string out){
	wav wi(orig);
	wav wo(out);

	wi.load();			
	wo.load();

	assert(wi.getNumSamples()==wo.getNumSamples()&&
		   wi.getNumOfChannels()==wo.getNumOfChannels());

	uint num_smp=wi.getNumSamples();	
	uint num_chan=wi.getNumOfChannels();	
	uint all_samp=num_smp*num_chan;

	double energy_signal=0;
	double energy_residual=0;

	short smp1,smp2;
	for(int i=0;i<num_smp;i++){
		for(int j=0;j<num_chan;j++){
			smp1=wi.get(i,j+1);
			smp2=wo.get(i,j+1);

			energy_signal+=((int)smp1*smp1)/(double)all_samp;
			energy_residual+=(((int)smp1-smp2)*((int)smp1-smp2))/(double)all_samp;
		}
	}
	return 10*std::log10(energy_signal/energy_residual);
}
/*
int main(int argc,char** argv){
	std::cout<<wav_utils::get_snr(string(argv[1]),string(argv[2]))<<std::endl;
}
*/
