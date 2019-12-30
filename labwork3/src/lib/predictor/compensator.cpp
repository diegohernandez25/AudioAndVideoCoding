#include "compensator.h"
#include <iostream> //FIXME remove

//assumes image comes already padded
compensator::compensator(uint macroblock_size,uint search_radius,uint lazy_score){
	this->search_radius=search_radius;
	this->lazy_score=lazy_score;
	this->macroblock_size=macroblock_size;
}


void compensator::apply_block_residual(cv::Mat& block,boost::circular_buffer<cv::Mat>& hist,cv::Vec4w block_meta){
	uint scalex=macroblock_size/block.cols;
	uint scaley=macroblock_size/block.rows;

	uint frame=block_meta[1];
	uint mvecx=block_meta[2];
	uint mvecy=block_meta[3];
	cv::Mat candidate=hist[frame](cv::Rect_<uint>(mvecx/scalex,mvecy/scaley,block.cols,block.rows));
	block-=candidate;
}

void compensator::restore_block(cv::Mat& block_residual,boost::circular_buffer<cv::Mat>& hist,cv::Vec3w mvec){
	uint scalex=macroblock_size/block_residual.cols;
	uint scaley=macroblock_size/block_residual.rows;

	uint frame=mvec[0];
	uint mvecx=mvec[1];
	uint mvecy=mvec[2];
	cv::Mat candidate=hist[frame](cv::Rect_<uint>(mvecx/scalex,mvecy/scaley,block_residual.cols,block_residual.rows));
	block_residual+=candidate;
}
cv::Mat compensator::find_matches(cv::Mat& curr,boost::circular_buffer<cv::Mat>& hist){
	cv::Mat	scores=cv::Mat::ones(curr.rows/macroblock_size,curr.cols/macroblock_size,CV_16UC4)*((1<<16)-1); 

	//TODO check the number of checked candidates
	//seems correct
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
block_iter: ;
		}
	}
	return scores;
}
