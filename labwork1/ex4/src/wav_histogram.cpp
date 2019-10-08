#include <iostream>

#include <map>

#include <sndfile.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "gnuplot.h"

// Read audio file with libsndfile
int readData(char* filename, SNDFILE* soundFileIn, SF_INFO& soundInfoIn,
			 sf_count_t& nSamples, short* sample, std::vector<short>& channels) {

	
	// As stated on the libsndfile documentation: "When opening a file for read, 
	// the format field should be set to zero before calling sf_open()." 
	soundInfoIn.format = 0;
	// Open the file
	soundFileIn = sf_open (filename, SFM_READ, &soundInfoIn);

	// No frames on the selected file
	if (soundInfoIn.frames == 0) {
		std::cerr << "Error: No frames found in file." << std::endl;
		sf_close(soundFileIn);
		return -1;
	}

	// Iterate over each frame
	for (int i = 0; i < soundInfoIn.frames ; i++) {
		// Read frame and send error if there is an attempt to read past
		// the end of the file
		if (sf_readf_short(soundFileIn, sample, nSamples) == 0) {
			std::cerr << "Error: Reached end of file." << std::endl;
			sf_close(soundFileIn);
			return -1;
		}
		// Add the value of each channel to the vector, alternated
		channels.push_back(sample[0]);
		channels.push_back(sample[1]);
	}
	// Close the file
	sf_close(soundFileIn);
	return 0;
}

// Calculates the histogram of each channel and the mono version
void buildHistogram(std::vector<short>& channels, std::map<short, int>& ch0,
					std::map<short, int>& ch1, std::map<short, int>& chm) {

	for (int i = 0; i < channels.size()/2; i+=2) {
		// Increment the number of occurrences of a certain value (key)
		ch0[channels[i]]++;
		// The values are alternated between channels, so check the next index for channel 1
		ch1[channels[i+1]]++;
		// The mono channel value equals the average of both channels
		chm[(channels[i]+channels[i+1])/2]++;
	}
}

// Shows plots by piping to Gnuplot
void showPlot(GnuplotPipe& gp, std::map<short, int> ch, std::string window_title, std::string title, int window_num) {
	gp.sendLine("set title '" + title + "'");
	gp.sendLine("set term wxt " + std::to_string(window_num) + " title '" + window_title + "'");
	gp.sendLine("plot '-'smooth freq with boxes");

	// The map already holds the number of occurrences for each value, so send the pair
	for(auto it = ch.cbegin(); it != ch.cend(); ++it) 
		gp.sendLine(std::to_string(it->first) + " " + std::to_string(it->second));

	gp.sendEndOfData();
}

int main(int argc, char* argv[]) {
	// Read parameters from arguments
	if (argc != 2) {
		std::cout << "Usage:" << std::endl
				  << "	vid_histogram <path_to_wav1>" << std::endl;
		return 0;
	}

	SNDFILE *soundFileIn;
	SF_INFO soundInfoIn;
	sf_count_t nSamples = 1;
	std::vector<short> channels;
	short sample[2];
	std::map<short, int> counter_channel0, counter_channel1, counter_mono;
	GnuplotPipe gp;

	// Read data from the provided file
	if (readData(argv[1], soundFileIn, soundInfoIn, nSamples,sample, channels) < 0)
		return -1;

	// Count the occurrence of each value in each channel and mono
	buildHistogram(channels, counter_channel0, counter_channel1, counter_mono);
	
	// Configure Gnuplot
	gp.sendLine("binwidth=1");
	gp.sendLine("set boxwidth binwidth");
	gp.sendLine("bin(x,width)=width*floor(x/width) + binwidth/2.0");

	// Plot the three histograms
	showPlot(gp, counter_channel0, std::string(argv[1]) + " - Channel 0", "Channel 0", 0);
	showPlot(gp, counter_channel1, std::string(argv[1]) + " - Channel 1", "Channel 0", 1);
	showPlot(gp, counter_mono,     std::string(argv[1]) + " - Mono", "Mono", 2);

	return 0;
}
