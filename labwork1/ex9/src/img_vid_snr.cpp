#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <numeric>

int pow2(uchar elem) {
	return int(elem) * int (elem);
}

double calculatePsnr(cv::Mat mat1, cv::Mat mat2) {
	cv::Mat diff_mat;
	cv::absdiff(mat1, mat2, diff_mat);
	int size = mat1.rows*mat2.cols;

	std::vector<int> vector(size);
	std::transform(diff_mat.begin<int>(), diff_mat.end<int>(),vector.begin(), pow2);
	
	int sum = std::accumulate(vector.begin(), vector.end(), 0);

	if (sum == 0) {
		std::cout << "Images are the same." << std::endl;
		return -1;
	}

	return 10*std::log10((255*255)/(double(sum)/size));
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
	
	// Compare images
	if (!isVid) {
		// Read images
		cv::Mat image_original, image_compressed;
		image_original = cv::imread(argv[2], cv::IMREAD_UNCHANGED );
		image_compressed = cv::imread(argv[3], cv::IMREAD_UNCHANGED );

		// Quit if not able to read images
		if (image_original.empty() || image_compressed.empty()) {
			std::cout << "Error: Image file could not be open." << std::endl;
			return -1;
		}

		if (image_original.cols != image_compressed.cols || image_original.rows != image_compressed.rows) {
			std::cout << "Error: Both images must have the same dimensions." << std::endl;
			return -1;
		}

		cv::Mat split_orig[3], split_compr[3];
		cv::split(image_original, split_orig);
		cv::split(image_compressed, split_compr);
		double psnr = 0, tmp;

		for (int i=0; i<3; i++) {
			tmp = calculatePsnr(split_orig[i], split_compr[i]);
			if (tmp < 0)
				return -1;
			psnr += tmp;
		}
		
		std::cout << "PSNR: " << psnr/3 << "dB" << std::endl;
	
	} else {
		// Initialize video
		cv::VideoCapture cap;
		cv::Mat frame;

		// Open capture
		cap.open(argv[2]);

		// Quit if unable to open capture
		if(!cap.isOpened()){
			std::cout << "Error opening video stream or file" << std::endl;
			return -1;
		}
	
		while (true) {
			// Get next frame
			cap >> frame;

			if (frame.empty())
				break;
		}

		cap.release();
	}
	
	return 0;
}
