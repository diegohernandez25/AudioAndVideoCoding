#ifndef H_MAT_GOLOMB_BITSTREAM
#define H_MAT_GOLOMB_BITSTREAM

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/core/mat.hpp>

#include "golomb_bitstream.h"

class mat_golomb_bitstream : public golomb_bitstream{
	public:
		mat_golomb_bitstream(uint initial_m,uint blk_size,uint calc_interval,bitstream_wrapper& bt);
		mat_golomb_bitstream(uint m,bitstream_wrapper& bt);

		//template <typename T> void write_mat(cv::Mat& m,bool is_signed);
		//template <typename T> void read_mat(cv::Mat& m,bool is_signed);
		void write_mat(cv::Mat& m,bool is_signed);
		void read_mat(cv::Mat& m,bool is_signed);

};

#endif
