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

template <typename T> void uniform_midrise(Mat& in,Mat& out,int bits){
	T mask= ~T(std::pow(2,sizeof(uchar)*8-bits)-1);
	bitwise_and(in,mask,out);
}

template <typename T> void uniform_midtread(Mat& in,Mat& out,int bits){
	T half = std::pow(2,sizeof(uchar)*8-bits)/2;
	add(in,half,out);
	uniform_midrise<T>(out,out,bits);
}

template <typename T> void lbg_init(vector<T>& codebook,int nr_vecs){
	codebook=vector<T>(nr_vecs);
	for(int i=0;i<codebook.size();i++)
		codebook.at(i)=Scalar(1,1,1)*((float)i/(codebook.size()-1)*255);
}
	
template <typename T> void lbg_calculate(Mat& in,vector<T>& codebook,Mat& result,int iter){
	uint nr_centroids=codebook.size();
	uint nr_chan=in.channels();
	Mat* tmp_calc = new Mat[nr_centroids]; 
	Mat* tmp_chans = new Mat[nr_chan];
	Mat* dists = new Mat[nr_centroids];
	result=Mat::zeros(in.rows,in.cols,CV_16U);
	Mat inp;
	in.convertTo(inp,CV_MAKETYPE(CV_32S,nr_chan));
	for(int i=0;i<nr_centroids;i++){
		tmp_calc[i]=Mat();
		dists[i]=Mat();
	}

	for(int it=0;it<iter;it++){
		for(int i=0;i<nr_centroids;i++){ //calculate distances
			absdiff(inp,codebook.at(i),tmp_calc[i]);	
			multiply(tmp_calc[i],tmp_calc[i],tmp_calc[i]);	
			split(tmp_calc[i],tmp_chans);
			for(int j=1;j<nr_chan;j++)
				add(tmp_chans[0],tmp_chans[j],tmp_chans[0]);
			tmp_chans[0].copyTo(dists[i]);
			//sqrt(tmp_chans[0],dists[i]); //not actually needed
		}

		for(int i=0;i<in.rows*in.cols;i++){ //min dists
			for(int j=1;j<nr_centroids;j++){
				if(dists[j].at<uint>(i)<dists[result.at<ushort>(i)].at<uint>(i)){
					result.at<ushort>(i)=j;
				}
			}	
		}

		T m,s;	
		for(int i=0;i<nr_centroids;i++){ //recalculate centroids
			meanStdDev(inp,m,s,result==i);	
			codebook.at(i)=m;
		}	

	}

	delete[] tmp_calc;
	delete[] tmp_chans;
	delete[] dists;
}

void lbg_apply(Mat& v,vector<Scalar>& codebook,Mat& out){
	for(int i=0;i<codebook.size();i++){
		out.setTo(codebook.at(i),v==i);
	}
}


int main(int argc, char* argv[]){

	if(argc!=4){
		cout<<"Usage: "<<argv[0]<<" file quantizebits out_folder"<<endl;
		cout<<"Linde-Buzo-Gray is calculated using the same amount of colors alowed by the quantizebits argument"<<endl;
		return 1;
	}

	Mat in = imread(argv[1],IMREAD_COLOR);
	Mat q(in.rows,in.cols,in.type());;	
	Mat q1(in.rows,in.cols,in.type());;	
	uniform_midrise<uchar>(in,q,atoi(argv[2]));	
	uniform_midtread<uchar>(in,q1,atoi(argv[2]));	

	vector<Scalar> codebook;
	Mat lbg_result;
	Mat q2(in.rows,in.cols,in.type());	
	lbg_init<Scalar>(codebook,pow(2,atoi(argv[2])*3));
	lbg_calculate<Scalar>(in,codebook,lbg_result,20);
	lbg_apply(lbg_result,codebook,q2);

	cout<<pow(2,atoi(argv[2])*3)<<" COLOURS MODE"<<endl;
	imshow("Original",in);
	imshow("MidRise",q);
	imshow("MidTread",q1);
	imshow("Linde-Buzo-Gray",q2);
	waitKey(0);

	imwrite(string(argv[3])+"/midrise.png",q);
	imwrite(string(argv[3])+"/midtread.png",q1);
	imwrite(string(argv[3])+"/linde_buzo_gray.png",q2);

	return 0;
}
