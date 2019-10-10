#include <iostream>

#include <unordered_map>
#include <math.h>

#include <boost/functional/hash.hpp>
#include <sndfile.h>

// Hash a pair using the boost library
struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const std::pair<T1, T2>& p) const { 
		std::size_t seed = 0;
        boost::hash_combine(seed, p.first);
        boost::hash_combine(seed, p.second);
        return seed; 
    } 
}; 

typedef std::unordered_map<std::pair<short,short>, int, hash_pair> pair_map;

// Read audio file with libsndfile and build the matrix
int readData(char* filename, SNDFILE* sound_file, SF_INFO& sound_info, 
			 pair_map& matrix, std::unordered_map<short, int>& accumulator) {

	// As stated on the libsndfile documentation: "When opening a file for read, 
	// the format field should be set to zero before calling sf_open()." 
	sound_info.format = 0;
	// Open the file
	sound_file = sf_open (filename, SFM_READ, &sound_info);
	short sample[2];
	std::pair<short, short> key;
	short previous, current;

	// No frames on the selected file
	if (sound_info.frames == 0) {
		std::cerr << "Error: No frames found in file." << std::endl;
		sf_close(sound_file);
		return -1;
	}

	// Read the first frame to save it as the previous frame, the rest of the processing
	// is skipped for the first frame.
	sf_readf_short(sound_file, sample, 1);
	// Iterate over each of the other frame
	for (int i = 1; i < sound_info.frames ; i++) {
		// The previous sample value is the average of both channels
		previous = (sample[0] + sample[1])/2;
		// Read one sample and send error if there is an attempt to read past
		// the end of the file
		if (sf_readf_short(sound_file, sample, 1) == 0) {
			std::cerr << "Error: Reached end of file." << std::endl;
			sf_close(sound_file);
			return -1;
		}
		// The current frame in mono
		current = (sample[0] + sample[1])/2;
		// The key to find the number of occurrences of a given sequence 
		key = std::make_pair(previous, current);
		// Increase the number of occurrences for this pair
		matrix[key]++;
		// Increase the number of occurences of the previous sample
		accumulator[previous]++;
	}

	// Close the file
	sf_close(sound_file);
	return sound_info.frames;
}

// Calculate the entropy of the file
double calculateEntropy(pair_map& matrix, std::unordered_map<short, int>& accumulator, int frames) {
	std::unordered_map<short, double> h;
	short previous;
	double pi, entropy = 0;

	// Iterate over the matrix
	for (auto it = matrix.begin(); it != matrix.end(); it++) {
		// The current symbol is the first element of the key
		previous = it->first.first;
		// The probability of the second symbol of the pair occuring after the current one
		pi = double(it->second)/accumulator[previous];
		// Calculate the expression for this sequence of symbols, adding to what has already been
		// done. In the end, this will give the average information per symbol.
		h[previous] += -pi * log2(pi);
	}

	// Iterate over each symbol
	for (auto it = h.begin(); it != h.end(); it++) {
		// The entropy of this file is the weighted average of the entropy of each symbol
		entropy += it->second*(accumulator[it->first]/double(frames));
	}

	return entropy;
}

int main(int argc, char* argv[]) {
	// Read parameters from arguments
	if (argc != 2) {
		std::cout << "Usage:" << std::endl
				  << "	vid_histogram <path_to_wav1>" << std::endl;
		return 0;
	}

	SNDFILE *sound_file;
	SF_INFO sound_info;
	pair_map matrix;
	std::unordered_map<short, int> accumulator;
	double frames;

	// Read data from the provided file
	frames = readData(argv[1], sound_file, sound_info, matrix, accumulator);

	if (frames < 0)
		return -1;

	std::cout << "Entropy of " << argv[1] << ": " << calculateEntropy(matrix, accumulator, frames) << std::endl;
	return 0;
}
