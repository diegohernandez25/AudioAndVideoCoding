#include <iostream>

#include <sndfile.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <map>
#include <gnuplot.h>

//int readData(char* filename, char)

int main(int argc, char* argv[]) {
	// Read paramets from arguments
	if (argc < 2) {
		std::cout << "Usage:" << std::endl
				  << "	vid_histogram <path_to_wav1> <path_to_wav2> ..." << std::endl;
		return 0;
	}

	SNDFILE *soundFileIn; /* Pointer for input sound file */
	SF_INFO soundInfoIn; /* Input sound file Info */

	int i;
	sf_count_t nSamples = 1;
	
	/* When opening a file for read, the format field should be set to zero
	 * before calling sf_open(). All other fields of the structure are filled
	 * in by the library
	 */	
	soundInfoIn.format = 0;
	soundFileIn = sf_open (argv[1], SFM_READ, &soundInfoIn);

	std::vector<short> channels;
	short sample[2];
	
	for (i = 0; i < soundInfoIn.frames ; i++) {
		if (sf_readf_short(soundFileIn, sample, nSamples) == 0){
			fprintf(stderr, "Error: Reached end of file\n");
			sf_close(soundFileIn);
			break;
		}
		channels.push_back(sample[0]);
		channels.push_back(sample[1]);
	}

	std::map<short, int> counter_channel0, counter_channel1, counter_mono;

	for (i = 0; i < channels.size()/2; i+=2) {
		counter_channel0[channels[i]]++;
		counter_channel1[channels[i+1]]++;
		// TODO rounding?
		counter_mono[(channels[i]+channels[i+1])/2]++;
	}

	GnuplotPipe gp;
	gp.sendLine("binwidth=1");
	gp.sendLine("set boxwidth binwidth");
	gp.sendLine("bin(x,width)=width*floor(x/width) + binwidth/2.0");
	gp.sendLine("set title 'Channel 0'");
	gp.sendLine("set term wxt 0 title 'Channel 0");
	gp.sendLine("plot '-'smooth freq with boxes");
	for(auto it = counter_channel0.cbegin(); it != counter_channel0.cend(); ++it) 
		gp.sendLine(std::to_string(it->first) + " " + std::to_string(it->second));

	gp.sendEndOfData();
	gp.sendLine("set title 'Channel 1'");
	gp.sendLine("set term wxt 1 title 'Channel 1'");
	gp.sendLine("plot '-'smooth freq with boxes");
	for(auto it = counter_channel1.cbegin(); it != counter_channel1.cend(); ++it) 
		gp.sendLine(std::to_string(it->first) + " " + std::to_string(it->second));
		
	gp.sendEndOfData();
	gp.sendLine("set title 'Mono'");
	gp.sendLine("set term wxt 2 title 'Mono'");
	gp.sendLine("plot '-'smooth freq with boxes");
	for(auto it = counter_mono.cbegin(); it != counter_mono.cend(); ++it) 
		gp.sendLine(std::to_string(it->first) + " " + std::to_string(it->second));

	return 0;
}
