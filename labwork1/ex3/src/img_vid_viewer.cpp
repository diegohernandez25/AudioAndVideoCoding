#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


int wait(int &i, int len) {
	char pressedKey = cv::waitKey(25);
	// If left key is pressed and it's not the first file
	if (pressedKey == 81 && i > 2) {
		i--;
		return -1;
	// If right key is pressed and it's not the last file
	} else if (pressedKey == 83 && i<len) {
		i++;
		return -1;
	// If "ESC" or "q" is pressed
	} else if (pressedKey == 113 || pressedKey == 27) {
		std::cout << 3 << std::endl;
		return 0;
	}
	return 1;
}

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

	cv::Mat image;
	cv::Mat frame;
	cv::VideoCapture cap;
	int i=2;
	int ret;

	while (true) {
		std::cout << "Showing " << argv[i] << std::endl;

		if (!isVid) {
			image = cv::imread(argv[i], cv::IMREAD_UNCHANGED );
			if(image.empty() )	{
				std::cout << "Image file could not be open." << std::endl;
				return -1;
			}
			cv::imshow("Image viewer", image );
		} else {
			cap.open(argv[i]);

			if(!cap.isOpened()){
				std::cout << "Error opening video stream or file" << std::endl;
				return -1;
			}
		}

		while (true) {
			if (isVid) {
				cap >> frame;

				if (frame.empty())
				break;

				cv::imshow( "Video player", frame );
			}
			ret = wait(i, argc-1);
			// Request to quit was received
			if (ret == 0) {
				return 0;
			// Request for next/previous file was received
			} else if (ret < 0) {
				break;
			}
		}
	}

	if (isVid) {	
		cap.release();
	}
		
	// Closes all the frames
	cv::destroyAllWindows();

	return 0;
}
