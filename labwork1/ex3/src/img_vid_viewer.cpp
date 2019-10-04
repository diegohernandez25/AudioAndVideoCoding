#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


int main(int argc, char* argv[]) {
	bool isVid;

	// read from args
	if (argc > 2) {
		if (argv[1] == std::string("-v")) {
			isVid = 1;
		} else if (argv[1] == std::string("-i")) {
			isVid = 0;
		} else {
			std::cout << "Invalid argument \"" << argv[1] << "\". Valid flags are: " << std::endl
					  << "	-v: video files" << std::endl
					  << "	-i: image files" << std::endl;
			return -1;
		}
	} else {
		std::cout << "Usage:" << std::endl
				  << "	-v <path_to_image1> <path_to_image2> ..." << std::endl
				  << "	or" << std::endl
				  << "	-i <path_to_video1> <path_to_video2> ..." << std::endl
			  	  << "\nPress ESC or \"q\" to quit. Press <left> and <right> arrows to move through different images" << std::endl;
		return 0;
	}

	if (isVid) {
		cv::VideoCapture cap("bus_cif.y4m");
		if(!cap.isOpened()){
			std::cout << "Error opening video stream or file" << std::endl;
			return -1;
		}
	} else {
		cv::Mat image;
		cv::namedWindow("Image viewer", cv::WINDOW_AUTOSIZE);
		int pressedKey;

		for (int i = 2; i < argc; i++) {
			image = cv::imread(argv[i], cv::IMREAD_UNCHANGED );
			pressedKey = 0;

			if(image.empty() )	{
				std::cout << "Image file could not be open." << std::endl;
				return -1;
			}

			cv::imshow("Image viewer", image );

			while (true) {
				pressedKey = cv::waitKey(0);
				std::cout << pressedKey << std::endl;
				if (pressedKey == 81 && i > 2) {
					i -= 2;
					break;
				} else if (pressedKey == 83 && i< argc-1) {
					break;
				} else if (pressedKey == 113 || pressedKey == 27) {
					return 0;
				}
			}
		}
		cv::destroyWindow("Image viewer" );
	}

	return 0;
}
