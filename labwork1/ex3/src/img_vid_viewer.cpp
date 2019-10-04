#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


int main(int argc, char* argv[]) {
	cv::Mat image;
	bool isVid;

	// read from args
	if (argc > 1) {
		if (argv[1] == std::string("-v")) {
			isVid = 1;
		} else if (argv[1] == std::string("-i")) {
			isVid = 0;
		} else {
			std::cout << "Invalid argument \"" << argv[1] << "\". Valid flags are: " << std::endl;
			std::cout << "	-v: video files" << std::endl;
			std::cout << "	-i: image files" << std::endl;
			return 0;
		}
	} else {
		std::cout << "Usage:" << std::endl;
		std::cout << "	-v <path_to_image1> <path_to_image2> ..." << std::endl;
		std::cout << "	or" << std::endl;
		std::cout << "	-i <path_to_video1> <path_to_video2> ..." << std::endl;
		return 0;
	}

	if (isVid) {
		// TODO
	} else {
		cv::namedWindow("Image viewer", cv::WINDOW_AUTOSIZE);
		for (int i = 2; i < argc; i++) {
			image = cv::imread(argv[i], cv::IMREAD_UNCHANGED );

			if(image.empty() )	{
				std::cout << "Image file could not be open." << std::endl;
				return -1;
			}

			cv::imshow("Image viewer", image );
			cv::waitKey(0);
		}
		cv::destroyWindow( "Image viewer" );
	}

	return 0;
}
