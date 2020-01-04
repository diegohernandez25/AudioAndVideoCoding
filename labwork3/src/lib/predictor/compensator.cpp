#include "compensator.h"
#include <iostream> //FIXME remove

//TODO logarithmic motion tracker

//assumes image comes already padded
compensator::compensator(uint macroblock_size,uint search_radius,uint lazy_score){
	this->search_radius=search_radius;
	this->lazy_score=lazy_score;
	this->macroblock_size=macroblock_size;
}


void compensator::apply_block_residual(cv::Mat& block,boost::circular_buffer<cv::Mat>& hist,cv::Vec4w block_meta){
	uint frame=block_meta[1];
	uint mvecx=block_meta[2];
	uint mvecy=block_meta[3];
	cv::Mat candidate=hist[frame](cv::Rect_<uint>(mvecx,mvecy,block.cols,block.rows));
	//block-=candidate;
	cv::subtract(block,candidate,block,cv::noArray(),CV_16S);
}

void compensator::restore_block(cv::Mat& block,cv::Mat& block_residual,boost::circular_buffer<cv::Mat>& hist,cv::Vec3w mvec){
	uint frame=mvec[0];
	uint mvecx=mvec[1];
	uint mvecy=mvec[2];
	cv::Mat candidate=hist[frame](cv::Rect_<uint>(mvecx,mvecy,block_residual.cols,block_residual.rows));
	//block_residual+=candidate;
	cv::add(block_residual,candidate,block,cv::noArray(),CV_8U);
}
cv::Mat compensator::find_matches(cv::Mat& curr,boost::circular_buffer<cv::Mat>& hist){
	cv::Mat	scores=cv::Mat::ones(std::ceil((float)curr.rows/macroblock_size),std::ceil((float)curr.cols/macroblock_size),CV_16UC4)*((1<<16)-1); 

	cv::Mat block,candidate;
	for(uint by=0;by<(uint)curr.rows;by+=macroblock_size){
		for(uint bx=0;bx<(uint)curr.cols;bx+=macroblock_size){
			uint macrox=macroblock_size;
			uint macroy=macroblock_size;
			bool print=false;
			if(bx+macroblock_size>(uint)curr.cols){//last macro blocks blocks might not be complete
				macrox=curr.cols-bx;
			}
			if(by+macroblock_size>(uint)curr.rows){//last macro blocks blocks might not be complete
				macroy=curr.rows-bx;
			}

			block=curr(cv::Rect_<uint>(bx,by,macrox,macroy));
			for(uint fr=0;fr<hist.size();fr++){
				cv::Mat frame=hist[fr];
				uint cy=(by<search_radius)?0:by-search_radius;
				for(;cy<=by+search_radius&&cy+macroy<(uint)curr.rows;cy++){
					uint cx=(bx<search_radius)?0:bx-search_radius;
					for(;cx<=bx+search_radius&&cx+macrox<(uint)curr.cols;cx++){
						candidate=frame(cv::Rect_<uint>(cx,cy,macrox,macroy));

						cv::Mat df; 
						cv::absdiff(block,candidate,df);							
						uint curr_score=cv::sum(df)[0];

						cv::Vec4w& entry = scores.at<cv::Vec4w>(by/macroblock_size,bx/macroblock_size);
						if(entry[0]>curr_score){
							entry[0]=curr_score; //Score
							entry[1]=fr; //Frame Nr
							entry[2]=cx; //MVecX
							entry[3]=cy; //MVecY
							if(print) std::cout<<cv::Rect_<uint>(cx,cy,macrox,macroy)<<frame.size()<<std::endl;

							if(curr_score<=lazy_score){ //jump to next block if satistied
								goto block_iter;
							}
						}
					}
				}
			}
			//if(print) std::cout<<"FINAL"<<cv::Rect_<uint>(scores.at<cv::Vec4w>(by/macroblock_size,bx/macroblock_size)[2],scores.at<cv::Vec4w>(by/macroblock_size,bx/macroblock_size)[3],macrox,macroy)<<std::endl;
block_iter: ;
		}
	}
	return scores;
}
