#include <iostream>

#include <numeric>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// Convert uchar element from the matrix to an int and calculate
// the power of two
int pow2(uchar elem) {
	return int(elem) * int (elem);
}

// Calculates the PSNR for a given pair of images
double calculatePsnr(cv::Mat mat1, cv::Mat mat2) {
	// Initialize matrix
	cv::Mat diff_mat;
	std::vector<int> unrolled_data(size);

	int size = mat1.rows*mat2.cols;

	// Calculate the modulus of the difference between both matrices
	cv::absdiff(mat1, mat2, diff_mat);
	// Calculate the power of two for each element of the matrix
	std::transform(diff_mat.begin<int>(), diff_mat.end<int>(), unrolled_data.begin(), pow2);
	// Sum all the elements
	int sum = std::accumulate(unrolled_data.begin(), unrolled_data.end(), 0);

	// If the sum of the differences is 0, the images are exactly the same
	if (sum == 0) {
		std::cout << "Images are the same." << std::endl;
		return -1;
	}

	// Expression for the PSNR
	return 10*std::log10((255*255)/(double(sum)/size));
}

// Process the given matrices
int processMats(cv::Mat orig, cv::Mat compr) {
	// Quit if not able to read images
	if (orig.empty() || compr.empty()) {
		std::cout << "Error: Image file could not be open." << std::endl;
		return -1;
	}

	// Compare the dimensions of both images
	if (orig.cols != compr.cols || orig.rows != compr.rows) {
		std::cout << "Error: Both images must have the same dimensions." << std::endl;
		return -1;
	}

	// Split matrices by color channel
	cv::Mat split_orig[3], split_compr[3];
	cv::split(orig, split_orig);
	cv::split(compr, split_compr);
	double psnr = 0, tmp;

	// Calculate PSNR for each channel
	for (int i=0; i<3; i++) {
		tmp = calculatePsnr(split_orig[i], split_compr[i]);
		if (tmp < 0)
			return -1;
		psnr += tmp;
	}
	
	// Return the average of the three channels
	std::cout << "PSNR: " << psnr/3 << "dB" << std::endl;
	return 0;
}

int main(int argc, char* argv[]) {
	bool isVid;

	// Read paramets from arguments
	if (argc == 4) {
		// Video input
		if (argv[1] == std::string("-v")) {
			isVid = 1;
		// Image input
		} else if (argv[1] == std::string("-i")) {
			isVid = 0;
		// Quit if given unknown argument
		} else {
			std::cout << "Invalid argument \"" << argv[1] << "\". Valid flags are: " << std::endl
					  << "	-v: video files" << std::endl
					  << "	-i: image files" << std::endl;
			return -1;
		}
	// Print usage instructions if not enough parameters are given
	} else {
		std::cout << "Usage:" << std::endl
				  << "	-i <path_to_original image> <path_to_compressed_image>" << std::endl
				  << "	or" << std::endl
				  << "	-v <path_to_original_video> <path_to_compressed_video>" << std::endl;
		return 0;
	}
	
	// Compare two static images
	if (!isVid) {
		// Read images
		cv::Mat image_original, image_compressed;
		image_original = cv::imread(argv[2], cv::IMREAD_UNCHANGED );
		image_compressed = cv::imread(argv[3], cv::IMREAD_UNCHANGED );

		// Process both images
		return processMats(image_original, image_compressed);
	
	// Compare two videos
	} else {
		// Initialize video
		cv::VideoCapture cap_orig, cap_compr;
		cv::Mat frame_orig, frame_compr;

		// Open capture
		cap_orig.open(argv[2]);
		cap_compr.open(argv[3]);

		// Quit if unable to open capture
		if(!cap_orig.isOpened() || !cap_compr.isOpened()){
			std::cout << "Error opening video stream or file" << std::endl;
			return -1;
		}

		// Cycle through all the frames
		while (true) {
			// Get next frame
			cap_orig >> frame_orig;
			cap_compr >> frame_compr;

			if (frame_orig.empty())
				break;
			
			// Process current frame from both sources
			processMats(frame_orig, frame_compr);

		}

		// Closes the capture
		cap_orig.release();
		cap_compr.release();
	}
	
	return 0;
}
