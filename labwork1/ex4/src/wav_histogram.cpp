#include <iostream>

#include <map>

#include <sndfile.h>

#include "gnuplot.h"

// Read audio file with libsndfile
int readData(char* filename, SNDFILE* sound_file, SF_INFO& sound_info,
			 std::vector<short>& samples) {

	// As stated on the libsndfile documentation: "When opening a file for read, 
	// the format field should be set to zero before calling sf_open()." 
	sound_info.format = 0;
	// Open the file
	sound_file = sf_open (filename, SFM_READ, &sound_info);
	short sample[2];

	// No frames on the selected file
	if (sound_info.frames == 0) {
		std::cerr << "Error: No frames found in file." << std::endl;
		sf_close(sound_file);
		return -1;
	}

	// Iterate over each frame
	for (int i = 0; i < sound_info.frames ; i++) {
		// Read one sample and send error if there is an attempt to read past
		// the end of the file
		if (sf_readf_short(sound_file, sample, 1) == 0) {
			std::cerr << "Error: Reached end of file." << std::endl;
			sf_close(sound_file);
			return -1;
		}
		// Add the value of each channel to the vector, alternated
		samples.push_back(sample[0]);
		samples.push_back(sample[1]);
	}
	// Close the file
	sf_close(sound_file);
	return 0;
}

// Calculates the histogram of each channel and the mono version
void buildHistogram(std::vector<short>& samples, std::map<short, int>& ch0,
					std::map<short, int>& ch1, std::map<short, int>& chm) {

	for (int i = 0; i < samples.size()/2; i+=2) {
		// Increment the number of occurrences of a certain value (key)
		ch0[samples[i]]++;
		// The values are alternated between channels, so check the next 
		// index for channel 1
		ch1[samples[i+1]]++;
		// The mono channel value equals the average of both channels
		chm[(samples[i]+samples[i+1])/2]++;
	}
}

// Shows plots by piping to Gnuplot
void showPlot(GnuplotPipe& gp, std::map<short, int> ch, std::string window_title,
			  std::string title, int window_num) {

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
				  << "	vid_histogram <path_to_wav>" << std::endl;
		return 0;
	}

	SNDFILE *sound_file;
	SF_INFO sound_info;
	std::vector<short> samples;
	std::map<short, int> hist_channel0, hist_channel1, hist_mono;
	GnuplotPipe gp;

	// Read data from the provided file
	if (readData(argv[1], sound_file, sound_info, samples) < 0)
		return -1;

	// Count the occurrence of each value in each channel and mono
	buildHistogram(samples, hist_channel0, hist_channel1, hist_mono);
	
	// Configure Gnuplot
	gp.sendLine("binwidth=1");
	gp.sendLine("set boxwidth binwidth");
	gp.sendLine("bin(x,width)=width*floor(x/width) + binwidth/2.0");

	// Plot the three histograms
	showPlot(gp, hist_channel0, std::string(argv[1]) + " - Channel 0", "Channel 0", 0);
	showPlot(gp, hist_channel1, std::string(argv[1]) + " - Channel 1", "Channel 0", 1);
	showPlot(gp, hist_mono,     std::string(argv[1]) + " - Mono", "Mono", 2);

	return 0;
}
