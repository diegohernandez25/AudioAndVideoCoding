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


int decode(args& cfg){

	bitstream bss(cfg.fileIn.c_str(),std::ios::binary|std::ios::in);	
	bitstream_wrapper bs(bss,true);	

	//Check magic
	char is_magic[strlen(magic)];
	bs.readNChars(is_magic,sizeof(is_magic));
	if(strncmp(magic,is_magic,strlen(magic))!=0) return -1;
	
	//Read Width/Height (Y4M)
	uint width=bs.readNBits(sizeof(uint)*8);
	uint height=bs.readNBits(sizeof(uint)*8);

	//Read Color Space/Interlace Mode (Y4M)
	uint8_t color_space=bs.readNBits(sizeof(uint8_t)*8);
	uint8_t interlace=bs.readNBits(sizeof(uint8_t)*8);

	//Read Frame Rate (Y4M)
	uint framerate[2];
	framerate[0]=bs.readNBits(sizeof(uint)*8);
	framerate[1]=bs.readNBits(sizeof(uint)*8);

	//Read Aspect Ratio (Y4M)
	uint aspect[2];
	aspect[0]=bs.readNBits(sizeof(uint)*8);
	aspect[1]=bs.readNBits(sizeof(uint)*8);

	//Read Number of Frames
	uint num_frames=bs.readNBits(sizeof(uint)*8);

	//Read Forced Prediction
	uint pred_mode=bs.readNBits(sizeof(uint8_t)*8);

	//Read block size, if present
	uint predBlockSize=0;
	if(pred_mode==9)
		predBlockSize=bs.readNBits(sizeof(uint)*8);

	//Create new video
	y4m out;
	out.init(width,height,color_space,predBlockSize);
	out.set_framerate(framerate[0],framerate[1]);
	out.set_aspect(aspect[0],aspect[1]);
	out.set_interlace(interlace);
	out.print_header();

	mat_golomb_bitstream gb_y(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	mat_golomb_bitstream gb_u(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	mat_golomb_bitstream gb_v(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	cv::Size block_size_y,block_size_uv;
	if(pred_mode==9){
		block_size_y=out.get_bsize_y();
		block_size_uv=out.get_bsize_uv();
	}
	else{
		block_size_y=out.get_y_size();
		block_size_uv=out.get_uv_size();
	}

	predictor pd_y(block_size_y.width,block_size_y.height);
	predictor pd_u(block_size_uv.width,block_size_uv.height);
	predictor pd_v(block_size_uv.width,block_size_uv.height);

	cv::Mat res_y(block_size_y,CV_16S);
	cv::Mat res_u(block_size_uv,CV_16S);
	cv::Mat res_v(block_size_uv,CV_16S);

	//Get Data
	for(uint f=0;f<num_frames;f++){
		cv::Mat y(out.get_pady_size(),CV_8U);
		cv::Mat u(out.get_paduv_size(),CV_8U);
		cv::Mat v(out.get_paduv_size(),CV_8U);

		pd_y.newFrame(&y);
		pd_u.newFrame(&u);
		pd_v.newFrame(&v);
		if(pred_mode==9){
			for(uint by=0;by*predBlockSize<(uint)y.rows;by++){
				for(uint bx=0;bx*predBlockSize<(uint)y.cols;bx++){
					uint bw_y=out.get_bsize_y().width;
					uint bh_y=out.get_bsize_y().height;
					uint bw_uv=out.get_bsize_uv().width;
					uint bh_uv=out.get_bsize_uv().height;

					//Get pred used
					uint8_t best_pred=bs.readNBits(4);

					//YUV residual
					gb_y.read_mat(res_y,true);
					gb_u.read_mat(res_u,true);
					gb_v.read_mat(res_v,true);

					//Reconstruct
					pd_y.reconstructBlock(bx*bw_y,by*bh_y,best_pred,&res_y);
					pd_u.reconstructBlock(bx*bw_uv,by*bh_uv,best_pred,&res_u);
					pd_v.reconstructBlock(bx*bw_uv,by*bh_uv,best_pred,&res_v);

				}
			}
		}
		else{
			//YUV residual
			gb_y.read_mat(res_y,true);
			gb_u.read_mat(res_u,true);
			gb_v.read_mat(res_v,true);

			//Reconstruct
			pd_y.reconstructBlock(0,0,pred_mode,&res_y);
			pd_u.reconstructBlock(0,0,pred_mode,&res_u);
			pd_v.reconstructBlock(0,0,pred_mode,&res_v);
		}

		out.push_frame(y,u,v);
	
	}
	out.save(cfg.fileOut);
	return 0;
}

int main(int argc,char** argv){
	args cfg = args(argc, argv, 0);
	return decode(cfg);
}
