#include <iostream>
#include "../lib/args/args.h"
#include "../lib/bitstream/bitstream.h"
#include "../lib/bitstream/bitstream_wrapper.h"
#include "../lib/predictor/predictor.h"
#include "../lib/golomb/mat_golomb_bitstream.h"
#include "../lib/y4m/y4m.h"

using namespace std;

const char* magic="VMAD0";

uint golomb_initial_m=5;
uint golomb_blk_size=128;
uint golomb_calc_interval=16;

//JPEG pred (0-7 Normal JPEG Preditors, 8 JPEG-LS, 9 Auto prediction)

void encode(args& cfg){
	y4m in;
	in.load(cfg.fileIn,cfg.jpegPredictor==9?cfg.blockSize:0);
	in.print_header();

	bitstream bss(cfg.fileOut.c_str(),std::ios::trunc|std::ios::binary|std::ios::out);	
	bitstream_wrapper bs(bss,true);	

	mat_golomb_bitstream gb_y(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	mat_golomb_bitstream gb_u(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	mat_golomb_bitstream gb_v(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	uint scalex=in.get_y().cols/in.get_u().cols;
	uint scaley=in.get_y().rows/in.get_u().rows;
	uint actual_block_size_x,actual_block_size_y;
	if(cfg.jpegPredictor==9){
		actual_block_size_x=cfg.blockSize;
		actual_block_size_y=cfg.blockSize;
	}
	else{
		actual_block_size_x=in.get_width();
		actual_block_size_y=in.get_height();
	}
	predictor pd_y(actual_block_size_x,actual_block_size_y);
	predictor pd_u(actual_block_size_x/scalex,actual_block_size_y/scaley);
	predictor pd_v(actual_block_size_x/scalex,actual_block_size_y/scaley);
	
	//Write magic
    bs.writeNChars((char*) magic,strlen(magic));

	//Write Width/Height (Y4M)
	bs.writeNBits(in.get_width(),sizeof(uint)*8);
	bs.writeNBits(in.get_height(),sizeof(uint)*8);

	//Write Color Space/Interlace Mode (Y4M)
	bs.writeNBits(in.get_color_space(),sizeof(uint8_t)*8);
	bs.writeNBits(in.get_interlace(),sizeof(uint8_t)*8);

	//Write Frame Rate (Y4M)
	bs.writeNBits(in.get_framerate()[0],sizeof(uint)*8);
	bs.writeNBits(in.get_framerate()[1],sizeof(uint)*8);

	//Write Aspect Ratio (Y4M)
	bs.writeNBits(in.get_aspect()[0],sizeof(uint)*8);
	bs.writeNBits(in.get_aspect()[1],sizeof(uint)*8);

	//Write Number of Frames
	bs.writeNBits(in.get_num_frames(),sizeof(uint)*8);

	//Write Forced Prediction
	bs.writeNBits(cfg.jpegPredictor,sizeof(uint8_t)*8);

	//Write block size, if needed
	if(cfg.jpegPredictor==9)
		bs.writeNBits(cfg.blockSize,sizeof(uint)*8);

	cv::Mat res_y(in.get_y().size(),CV_16S);
	cv::Mat res_u(in.get_u().size(),CV_16S);
	cv::Mat res_v(in.get_v().size(),CV_16S);
	//Add Data
	do{
		cv::Mat y=in.get_y().clone();
		cv::Mat u=in.get_u().clone();
		cv::Mat v=in.get_v().clone();

		pd_y.newFrame(&y);
		pd_u.newFrame(&u);
		pd_v.newFrame(&v);

		if(cfg.jpegPredictor==9){
			for(uint by=0;by<y.rows;by+=cfg.blockSize){
				for(uint bx=0;bx<y.cols;bx+=cfg.blockSize){
					cv::Mat blk_y = res_y(cv::Rect_<uint>(bx,by,cfg.blockSize,cfg.blockSize));
					cv::Mat blk_u = res_u(cv::Rect_<uint>(bx/scalex,by/scaley,cfg.blockSize/scalex,cfg.blockSize/scaley));
					cv::Mat blk_v = res_v(cv::Rect_<uint>(bx/scalex,by/scaley,cfg.blockSize/scalex,cfg.blockSize/scaley));

					uint8_t best_pred=std::get<0>(pd_y.calcBestResiduals(bx,by,&blk_y));
					pd_u.calcBlockResiduals(bx/scalex,by/scaley,best_pred,&blk_u);
					pd_v.calcBlockResiduals(bx/scalex,by/scaley,best_pred,&blk_v);

					bs.writeNBits(best_pred,4);	
					gb_y.write_mat(blk_y,true);	
					gb_u.write_mat(blk_u,true);	
					gb_v.write_mat(blk_v,true);	
				}		
			}		
		}
		else{

			//Forced mode
			pd_y.calcBlockResiduals(0,0,cfg.jpegPredictor,&res_y);
			pd_u.calcBlockResiduals(0,0,cfg.jpegPredictor,&res_u);
			pd_v.calcBlockResiduals(0,0,cfg.jpegPredictor,&res_v);

			gb_y.write_mat(res_y,true);	
			gb_u.write_mat(res_u,true);	
			gb_v.write_mat(res_v,true);	
		}

	}while(in.next_frame());	
}

int main(int argc,char** argv){
	args cfg = args(argc, argv, 1);
	encode(cfg);
}
