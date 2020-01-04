#include "mat_golomb_bitstream.h"

mat_golomb_bitstream::mat_golomb_bitstream(uint m,bitstream_wrapper& bt) : golomb_bitstream(m,bt){}

mat_golomb_bitstream::mat_golomb_bitstream(uint initial_m,uint blk_size,uint calc_interval,bitstream_wrapper& bt) : golomb_bitstream(initial_m,blk_size,calc_interval,bt){}
/*
template <typename T> void mat_golomb_bitstream::write_mat(cv::Mat& m,bool is_signed){
	for(uint y=0;y<m.rows;y++){
		for(uint x=0;x<m.cols;x++){
			if(is_signed) write_signed_val(m.at<T>(y,x));
			else write_val(m.at<T>(y,x));
		}
	}
}

template <typename T> void mat_golomb_bitstream::read_mat(cv::Mat& m,bool is_signed){
	for(uint y=0;y<m.rows;y++){
		for(uint x=0;x<m.cols;x++){
			if(is_signed) m.at<T>(y,x)=read_signed_val();
			else m.at<T>(y,x)=read_val();
		}
	}
*/
void mat_golomb_bitstream::write_mat(cv::Mat& m,bool is_signed){
	for(uint y=0;y<(uint)m.rows;y++){
		for(uint x=0;x<(uint)m.cols;x++){
			if(is_signed) write_signed_val(m.at<short>(y,x));
			else write_val(m.at<ushort>(y,x));
		}
	}
}

void mat_golomb_bitstream::read_mat(cv::Mat& m,bool is_signed){
	for(uint y=0;y<(uint)m.rows;y++){
		for(uint x=0;x<(uint)m.cols;x++){
			if(is_signed) m.at<short>(y,x)=read_signed_val();
			else m.at<ushort>(y,x)=read_val();
		}
	}
}
