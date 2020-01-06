#include <iostream>
#include "../lib/args/args.h"
#include "../lib/bitstream/bitstream.h"
#include "../lib/bitstream/bitstream_wrapper.h"
#include "../lib/predictor/predictor.h"
#include "../lib/golomb/mat_golomb_bitstream.h"
#include "../lib/y4m/y4m.h"
#include "../lib/dct/dct.h"


using namespace std;

const char* magic="VMAD4";

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

	golomb_bitstream gb_y(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	golomb_bitstream gb_y_zeros(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	golomb_bitstream gb_u(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	golomb_bitstream gb_u_zeros(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	golomb_bitstream gb_v(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	golomb_bitstream gb_v_zeros(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	cv::Size block_size_y,block_size_uv;
	if(cfg.jpegPredictor==9){
		block_size_y=in.get_bsize_y();
		block_size_uv=in.get_bsize_uv();
	}
	else{
		block_size_y=in.get_y_size();
		block_size_uv=in.get_uv_size();
	}

	cv::Size uv_size=in.get_uv_size();

  dct dct_y( int(in.get_height()), int(in.get_width()),block_size_y,3);
	dct dct_u( int(uv_size.height), int(uv_size.width),block_size_uv,3);
	dct dct_v( int(uv_size.height), int(uv_size.width),block_size_uv,3);

	vector<tuple<short,short>> rle_y, rle_u, rle_v;

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

	cv::Mat res_y(block_size_y,CV_16S);
	cv::Mat res_u(block_size_uv,CV_16S);
	cv::Mat res_v(block_size_uv,CV_16S);


	do{
		cv::Mat y=in.get_y();
		cv::Mat u=in.get_u();
		cv::Mat v=in.get_v();
		for(uint by=0;by*cfg.blockSize<(uint)y.rows;by++){
			for(uint bx=0;bx*cfg.blockSize<(uint)y.cols;bx++){
				uint bw_y=in.get_bsize_y().width;
				uint bh_y=in.get_bsize_y().height;
				uint bw_uv=in.get_bsize_uv().width;
				uint bh_uv=in.get_bsize_uv().height;

				cv::Mat res_y = y(cv::Rect(bx*bw_y,by*bh_y, bw_y, bh_y));
				cv::Mat res_u = u(cv::Rect(bx*bw_uv,by*bh_uv, bw_uv, bh_uv));
				cv::Mat res_v = v(cv::Rect(bx*bw_uv,by*bh_uv, bw_uv, bh_uv));

				rle_y=dct_y.dct_quant_rle_blck(res_y, false);
				rle_u=dct_u.dct_quant_rle_blck(res_u, false);
				rle_v=dct_v.dct_quant_rle_blck(res_v, false);

				for(tuple<short,short> t:rle_y){
					gb_y_zeros.write_signed_val(std::get<0>(t));
					gb_y.write_signed_val(std::get<1>(t));
				}
				for(tuple<short,short> t:rle_u){
					gb_u_zeros.write_signed_val(std::get<0>(t));
					gb_u.write_signed_val(std::get<1>(t));
				}
				for(tuple<short,short> t:rle_v){
					gb_v_zeros.write_signed_val(std::get<0>(t));
					gb_v.write_signed_val(std::get<1>(t));
				}
			}
		}
	}while(in.next_frame());
}


int main(int argc,char** argv){
	args cfg = args(argc, argv, 3);
	encode(cfg);
}
