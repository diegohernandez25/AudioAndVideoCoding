#include <iostream>
#include "../lib/args/args.h"
#include "../lib/y4m/y4m.h"

using namespace std;

int main(int argc,char** argv){
	args cfg = args(argc, argv, 3);
	y4m cap;
	cv::Mat frame;

	if(!cap.load(cfg.fileIn,0)){
		std::cout << "ERROR: couldn't open video file" << std::endl;
		return -1;
	}

	while (true) {
		// Get next frame
		frame=cap.get_bgr();

		if (frame.empty())
			break;

		// TODO encode lossy

		if(!cap.next_frame())
			break;
	}
}
