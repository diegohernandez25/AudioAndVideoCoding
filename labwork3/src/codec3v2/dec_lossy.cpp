#include <iostream>
#include <cmath>
#include <tuple>
#include "../lib/args/args.h"
#include "../lib/bitstream/bitstream.h"
#include "../lib/bitstream/bitstream_wrapper.h"
#include "../lib/predictor/predictor.h"
#include "../lib/golomb/mat_golomb_bitstream.h"
#include "../lib/y4m/y4m.h"
#include "../lib/dct/dct.h"


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

	//Read block size
	uint predBlockSize=bs.readNBits(sizeof(uint)*8);

	//Create new video
	y4m out;
	out.init(width,height,color_space,predBlockSize);
	out.set_framerate(framerate[0],framerate[1]);
	out.set_aspect(aspect[0],aspect[1]);
	out.set_interlace(interlace);
	out.print_header();

	golomb_bitstream gb_y(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	golomb_bitstream gb_y_zeros(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	golomb_bitstream gb_u(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	golomb_bitstream gb_u_zeros(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	golomb_bitstream gb_v(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	golomb_bitstream gb_v_zeros(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	cv::Size block_size_y,block_size_uv;

	block_size_y=out.get_bsize_y();
	block_size_uv=out.get_bsize_uv();

	cv::Size uv_size=out.get_uv_size();
	dct dct_y = dct(int(height), int(width), block_size_y,3);
	dct dct_u = dct(int(uv_size.height), int(uv_size.width), block_size_uv,3);
	dct dct_v = dct(int(uv_size.height), int(uv_size.width), block_size_uv,3);

	vector<tuple<short,short>> rle_y, rle_u, rle_v;
	rle_y.reserve(block_size_y.height*block_size_y.width);
	rle_u.reserve(block_size_uv.height*block_size_uv.width);
	rle_v.reserve(block_size_uv.height*block_size_uv.width);

	cv::Mat res_y(block_size_y,CV_16S);
	cv::Mat res_u(block_size_uv,CV_16S);
	cv::Mat res_v(block_size_uv,CV_16S);

	//Get Data
	for(uint f=0;f<num_frames;f++){

		cv::Mat y(out.get_pady_size(),CV_8U);
		cv::Mat u(out.get_paduv_size(),CV_8U);
		cv::Mat v(out.get_paduv_size(),CV_8U);


		for(uint by=0;by*predBlockSize<(uint)y.rows;by++){
			for(uint bx=0;bx*predBlockSize<(uint)y.cols;bx++){
				uint bw_y=out.get_bsize_y().width;
				uint bh_y=out.get_bsize_y().height;
				uint bw_uv=out.get_bsize_uv().width;
				uint bh_uv=out.get_bsize_uv().height;

				rle_y.clear();
				rle_u.clear();
				rle_v.clear();

				//YUV residual
				short n_zeros,val;
				while (true) {
					n_zeros=gb_y_zeros.read_signed_val();
					val=gb_y.read_signed_val();
					rle_y.push_back(make_tuple(n_zeros,val));
					if(n_zeros==0 && val==0) break;
				}
				while(true){
					n_zeros=gb_u_zeros.read_signed_val();
					val=gb_u.read_signed_val();
					rle_u.push_back(make_tuple(n_zeros,val));
					if(n_zeros==0 && val==0) break;
				}
				while(true){
					n_zeros=gb_v_zeros.read_signed_val();
					val=gb_v.read_signed_val();
					rle_v.push_back(make_tuple(n_zeros,val));
					if(n_zeros==0 && val==0) break;
				}

				res_y=dct_y.reverse_dct_quant_rle_blck(rle_y, false);
				res_u=dct_u.reverse_dct_quant_rle_blck(rle_u, false);
				res_v=dct_v.reverse_dct_quant_rle_blck(rle_v, false);

				//Copy block to frame.
				res_y.copyTo(y(cv::Rect(bx*bw_y,by*bh_y,bw_y,bh_y)));
				res_u.copyTo(u(cv::Rect(bx*bw_uv,by*bh_uv,bw_uv,bh_uv)));
				res_v.copyTo(v(cv::Rect(bx*bw_uv,by*bh_uv,bw_uv,bh_uv)));
			}
		}
		//Put frame into video.
		out.push_frame(y,u,v);

	}
	out.save(cfg.fileOut);
	return 0;
}

int main(int argc,char** argv){

	args cfg = args(argc, argv, 0);
	return decode(cfg);
}
