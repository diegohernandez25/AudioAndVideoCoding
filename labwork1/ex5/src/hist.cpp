#include <iostream>
#include <vector>
#include <cassert>
#include <alloca.h>
#include <cmath>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

template <typename T> void create_hist(Mat& in,Mat& out){ //accepts unidimentional input and output
	assert(sizeof(T)==in.elemSize() &&
		   in.channels()==1 &&
		   out.channels()==1);

	uint cs=pow(2,in.elemSize()*8); //cell size
	uint acc_size=(uint)out.cols>cs ? cs:out.cols;
	uint* acc=(uint*) alloca(sizeof(uint)*acc_size);
	memset(acc,0,sizeof(uint)*acc_size);

	T* init=(T*) in.ptr();
	uint npixels=in.cols*in.rows;

	if(acc_size==cs)
		for(T* p=init;p<init+npixels;p++){
			acc[*p]++;
		}
	else{
		for(T* p=init;p<init+npixels;p++){
			acc[(int)((float)*p/cs*acc_size+0.5)]++; //creates artifacts when <256?
		}
	}

	uint max=0; //more efficient to test after all insertions
	for(uint i=0;i<acc_size;i++)
		if(acc[i]>max)
			max=acc[i];


	float thickness=(float)out.cols/acc_size;
	float scale_factor=(float)out.rows/max;

	//draw hist
	for(uint i=0;i<acc_size;i++){
		rectangle(out,
			Point2i(i*thickness+0.5,out.rows),
			Point2i((i+1)*thickness-0.5,out.rows-scale_factor*acc[i]),
			255,FILLED);
		rectangle(out,
			Point2i(i*thickness+0.5,0),
			Point2i((i+1)*thickness-0.5,out.rows-scale_factor*acc[i]-1),
			0,FILLED);
	}
}


void create_hist_old(Mat& in,Mat& out){ //accepts unidimentional input and output
	assert(out.cols%256==0&&
			in.channels()==1&&
			out.channels()==1);

	uchar* init=in.ptr();
	uint npixels=in.cols*in.rows;
	uint acc[256]={0}; //2 "to the power of" sizeof(uchar)*8

	for(uchar* p=init;p<init+npixels;p++){
		acc[*p]++;
	}

	uint max=0; //more efficient to test after all insertions
	for(int i=0;i<256;i++)
		if(acc[i]>max)
			max=acc[i];

	int thickness=out.cols/256;
	float scale_factor=(float)out.rows/max;

	//draw hist
	for(int i=0;i<256;i++){
		rectangle(out,
				Point2i(i*thickness,out.rows),
				Point2i((i+1)*thickness-1,out.rows-scale_factor*acc[i]),
				255,FILLED);
	}

}

