#include <iostream>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;


template<typename T> void copy_image(Mat& in,Mat& out){
	out=Mat(in.rows,in.cols,in.type());
	for(int row=0;row<in.rows;row++)
		for(int col=0;col<in.cols;col++)
			out.at<T>(row,col)=in.at<T>(row,col); 
}

int main(int argc, char* argv[]){

	if(argc!=3){
		cout<<"Usage: "<<argv[0]<<" infile outfile"<<endl;
		return -1;
	}

	Mat in = imread(argv[1],IMREAD_COLOR); //returns RGB 8bit depth 
	if(in.empty()){
		cout<<"Could not open image"<<endl;
	    return -2;
	}

	Mat out;
	copy_image<Vec3b>(in,out);	
	cv::imwrite(argv[2],out);

	cv::imshow("Image",out);
	cv::waitKey(0);

	return 0;
}
