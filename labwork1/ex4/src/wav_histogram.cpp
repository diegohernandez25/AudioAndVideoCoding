#include <iostream>

#include <map>

#include <sndfile.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "gnuplot.h"

int readData(char* filename, SNDFILE* soundFileIn, SF_INFO& soundInfoIn,
			 sf_count_t& nSamples, short* sample, std::vector<short>& channels) {

	soundInfoIn.format = 0;
	soundFileIn = sf_open (filename, SFM_READ, &soundInfoIn);
	for (int i = 0; i < soundInfoIn.frames ; i++) {
		if (sf_readf_short(soundFileIn, sample, nSamples) == 0){
			std::cerr << "Error: Reached end of file" << std::endl;
			sf_close(soundFileIn);
			return -1;
		}
		channels.push_back(sample[0]);
		channels.push_back(sample[1]);
	}
	return 0;
}

void buildHistogram(std::vector<short>& channels, std::map<short, int>& ch0,
					std::map<short, int>& ch1, std::map<short, int>& chm) {

	for (int i = 0; i < channels.size()/2; i+=2) {
		ch0[channels[i]]++;
		ch1[channels[i+1]]++;
		chm[(channels[i]+channels[i+1])/2]++;
	}
}

void showPlot(GnuplotPipe& gp, std::map<short, int> ch, std::string window_title, std::string title, int window_num) {
	gp.sendLine("set title '" + title + "'");
	gp.sendLine("set term wxt " + std::to_string(window_num) + " title '" + window_title + "'");
	gp.sendLine("plot '-'smooth freq with boxes");
	for(auto it = ch.cbegin(); it != ch.cend(); ++it) 
		gp.sendLine(std::to_string(it->first) + " " + std::to_string(it->second));

	gp.sendEndOfData();
}

int main(int argc, char* argv[]) {
	// Read parameters from arguments
	if (argc < 2) {
		std::cout << "Usage:" << std::endl
				  << "	vid_histogram <path_to_wav1> <path_to_wav2> ..." << std::endl;
		return 0;
	}

	SNDFILE *soundFileIn; /* Pointer for input sound file */
	SF_INFO soundInfoIn; /* Input sound file Info */
	sf_count_t nSamples = 1;
	std::vector<short> channels;
	short sample[2];
	std::map<short, int> counter_channel0, counter_channel1, counter_mono;
	GnuplotPipe gp;

	if (readData(argv[1], soundFileIn, soundInfoIn, nSamples,sample, channels) < 0)
		return -1;

	buildHistogram(channels, counter_channel0, counter_channel1, counter_mono);
	
	gp.sendLine("binwidth=1");
	gp.sendLine("set boxwidth binwidth");
	gp.sendLine("bin(x,width)=width*floor(x/width) + binwidth/2.0");

	showPlot(gp, counter_channel0, std::string(argv[1]) + " - Channel 0", "Channel 0", 0);
	showPlot(gp, counter_channel1, std::string(argv[1]) + " - Channel 1", "Channel 0", 1);
	showPlot(gp, counter_mono,     std::string(argv[1]) + " - Mono", "Mono", 2);

	return 0;
}
