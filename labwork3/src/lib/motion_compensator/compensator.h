#ifndef H_COMPENSATOR
#define H_COMPENSATOR

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/core/mat.hpp>
#include <boost/circular_buffer.hpp>

class compensator{
	public:
		compensator(uint search_radius,uint search_depth,uint lazy_score);

		cv::Mat find_matches(cv::Mat& curr,boost::circular_buffer<cv::Mat>& hist); //Mat -> (score,frame,mvecx,mvecy)

	private:
		uint search_radius;
		uint lazy_score;
		uint macroblock_size;
};

#endif
