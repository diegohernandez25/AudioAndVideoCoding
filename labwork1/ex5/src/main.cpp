#include <iostream>
#include <vector>
#include <cassert>
#include <alloca.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "hist.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[]){

	if(argc!=4){
		cout<<"Usage: "<<argv[0]<<" file hist_height hist_width"<<endl;
		return 1;
	}

	int rows=atoi(argv[2]);
	int cols=atoi(argv[3]);

	VideoCapture vc(argv[1]);
	Mat in;
	vc >> in;

	Mat out(rows,cols,in.type());
	Mat chs[3],hists[3],hout[3],gray,grayhist(rows,cols,CV_8U);
	Mat z = Mat::zeros(rows,cols, CV_8U);

	while(!in.empty()){

		//Channel by channel
		split(in,chs);
		vector<Mat> v;
		for(int i=0;i<3;i++){
			hists[i]=Mat(out.rows,out.cols,CV_8U);
			hout[i]=Mat(out.rows,out.cols,CV_8UC3);
			create_hist<uchar>(chs[i],hists[i]);

			v=vector<Mat>();
			v.push_back(z);
			v.push_back(z);
			v.push_back(hists[i]);
			rotate(v.begin(),v.end()-i-1,v.end()); //BGR
			merge(v,hout[i]);
		}
		merge(hists,3,out);

		//Gray
		cv::cvtColor(in, gray, COLOR_BGR2GRAY);
		create_hist<uchar>(gray,grayhist);

		imshow("Original",in);
		imshow("B", hout[0]);
		imshow("G", hout[1]);
		imshow("R", hout[2]);
		imshow("Greyscale",grayhist); //FIXME seems wrong?
		imshow("Sum",out);
		
		if(waitKey(40)=='q')
			break;

		vc>>in;
	}
	waitKey(0);
	
    //cv::namedWindow( "1", cv::WINDOW_AUTOSIZE );
    //cv::destroyWindow( "Display window" );

}
