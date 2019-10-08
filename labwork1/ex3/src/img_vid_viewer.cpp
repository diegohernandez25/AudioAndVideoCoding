#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// Waits for a key input and handles it
int wait(int &i, int len) {
	char pressedKey = cv::waitKey(25);
	// If left key is pressed and it's not the first file, go to the previous one
	if (pressedKey == 81 && i > 2) {
		i--;
		return -1;
	// If right key is pressed and it's not the last file, go to the next one
	} else if (pressedKey == 83 && i<len) {
		i++;
		return -1;
	// If "ESC" or "q" is pressed, quit
	} else if (pressedKey == 113 || pressedKey == 27) {
		std::cout << 3 << std::endl;
		return 0;
	}
	return 1;
}

int main(int argc, char* argv[]) {
	bool isVid;

	// Read paramets from arguments
	if (argc > 2) {
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
				  << "	-i <path_to_image1> <path_to_image2> ..." << std::endl
				  << "	or" << std::endl
				  << "	-v <path_to_video1> <path_to_video2> ..." << std::endl
			  	  << "\nPress ESC or \"q\" to quit. Press <left> and <right> arrows to move through different files" << std::endl;
		return 0;
	}

	// Variable intialization
	cv::Mat image;
	cv::Mat frame;
	cv::VideoCapture cap;
	int i=2;
	int ret;

	// Keeps showing videos/image til a stop request is sent
	while (true) {
		std::cout << "Displaying " << argv[i] << std::endl;

		// Initialize image
		if (!isVid) {
			// Read image
			image = cv::imread(argv[i], cv::IMREAD_UNCHANGED );
			// Quit if not able to read image
			if(image.empty() )	{
				std::cout << "Image file could not be open." << std::endl;
				return -1;
			}
			// Show image in a window
			cv::imshow("Image viewer", image );
		// Initialize video
		} else {
			// Open capture
			cap.open(argv[i]);

			// Quit if unable to open capture
			if(!cap.isOpened()){
				std::cout << "Error opening video stream or file" << std::endl;
				return -1;
			}
		}

		while (true) {
			// If playing video, cycle through the frames as well
			if (isVid) {
				// Get next frame
				cap >> frame;

				if (frame.empty())
					break;

				// Show the frame in a window
				cv::imshow( "Video player", frame );
			}

			// Check for input to quit or to show next or previous file
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

	// Closes the capture
	if (isVid) {	
		cap.release();
	}
		
	// Closes all the windows
	cv::destroyAllWindows();

	return 0;
}
