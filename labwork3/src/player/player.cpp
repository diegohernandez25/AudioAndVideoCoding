#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "../lib/y4m/y4m.h"

//Based on exercise 1 of labwork 1

// Waits for a key input and handles it
int wait(int &i, int len,float fps) {
	char pressedKey = cv::waitKey(1000/fps);
	// If left key is pressed and it's not the first file, go to the previous one
	if (pressedKey == 81 && i > 1) {
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

	// Print usage instructions if not enough parameters are given
	if (argc < 2) {
		std::cout << 
			"Usage: "<< argv[0] << " <path_to_video1> <path_to_video2> ..." << std::endl
			<< "\nPress ESC or \"q\" to quit. Press <left> and <right> arrows to move through different files" << std::endl;
		return 0;
	}

	// Variable intialization
	cv::Mat frame;
	y4m cap;
	float fps;
	//cv::VideoCapture cap;
	int i=1;
	int ret;

	// Keeps showing videos until a stop request is sent
	while (true) {
		std::cout << "Displaying " << argv[i] << std::endl;

		// Quit if unable to open capture
		std::string fl(argv[i]);
		if(!cap.load(fl,0)){
			std::cout << "Error opening video file" << std::endl;
			return -1;
		}
		fps=cap.get_fps();

		while (true) {
			// Cycle through the frames as well
			// Get next frame
			frame=cap.get_bgr();

			if (frame.empty())
				break;

			// Show the frame in a window
			cv::imshow( "Video player", frame );

			// Check for input to quit or to show next or previous file
			ret = wait(i, argc-1,fps);
			// Request to quit was received
			if (ret == 0) {
				return 0;
			// Request for next/previous file was received
			} else if (ret < 0) {
				break;
			}
			if(!cap.next_frame()) break;
		}
	}

	// Closes all the windows
	cv::destroyAllWindows();

	return 0;
}
