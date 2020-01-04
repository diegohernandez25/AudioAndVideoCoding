#ifndef H_COMPENSATOR
#define H_COMPENSATOR

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/core/mat.hpp>
#include <boost/circular_buffer.hpp>

class compensator{
	public:
		compensator(){};
		compensator(uint macroblock_size,uint search_radius,uint lazy_score);

		cv::Mat find_matches(cv::Mat& curr,boost::circular_buffer<cv::Mat>& hist); //Mat -> (score,frame,mvecx,mvecy)
		void apply_block_residual(cv::Mat& block,boost::circular_buffer<cv::Mat>& hist,cv::Vec4w block_meta); //Vec4w -> (score,frame,mvecx,mvecy)
		void restore_block(cv::Mat& block,cv::Mat& block_residual,boost::circular_buffer<cv::Mat>& hist,cv::Vec3w mvec); //Vec3w -> (frame,mvecx,mvecy)

	private:
		uint search_radius;
		uint lazy_score;
		uint macroblock_size;
};

#endif
