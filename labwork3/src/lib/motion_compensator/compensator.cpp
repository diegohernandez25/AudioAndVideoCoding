#include "compensator.h"
#include <iostream> //FIXME remove

compensator::compensator(uint macroblock_size,uint search_radius,uint lazy_score){
	this->search_radius=search_radius;
	this->lazy_score=lazy_score;
	this->macroblock_size=macroblock_size;
}


//assumes image comes already padded
cv::Mat compensator::find_matches(cv::Mat& curr,boost::circular_buffer<cv::Mat>& hist){
	cv::Mat	scores=cv::Mat::ones(curr.rows/macroblock_size,curr.cols/macroblock_size,CV_16UC4)*((1<<16)-1); 
	

	//TODO check the number of checked candidates
	cv::Mat block,candidate;
	for(uint bx=0;bx<curr.cols;bx+=macroblock_size){
		for(uint by=0;by<curr.rows;by+=macroblock_size){
			block=curr(cv::Rect_<uint>(bx,by,macroblock_size,macroblock_size));
			for(uint fr=0;fr<hist.size();fr++){
				cv::Mat frame=hist[fr];
				uint cx=(bx<search_radius)?0:bx-search_radius;
				for(;cx<=bx+search_radius&&cx+macroblock_size<curr.cols;cx++){
					uint cy=(by<search_radius)?0:by-search_radius;
					for(;cy<=by+search_radius&&cy+macroblock_size<curr.rows;cy++){
						//std::cout<<cx<<","<<cy<<","<<cx+macroblock_size<<","<<cy+macroblock_size<<std::endl;
						candidate=frame(cv::Rect_<uint>(cx,cy,macroblock_size,macroblock_size));

						cv::Mat df; 
						cv::absdiff(block,candidate,df);							
						uint curr_score=cv::sum(df)[0];

						cv::Vec4w& entry = scores.at<cv::Vec4w>(by/macroblock_size,bx/macroblock_size);
						if(entry[0]>curr_score){
							entry[0]=curr_score; //Score
							entry[1]=fr; //Frame Nr
							entry[2]=cx; //MVecX
							entry[3]=cy; //MVecY

							if(curr_score<=lazy_score){ //jump to next block if satistied
								goto block_iter;
							}
						}
					}
				}
			}
block_iter: continue; //FIXME
		}
	}
	return scores;
}
