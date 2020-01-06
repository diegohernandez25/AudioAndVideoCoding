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
    if(!in.load(cfg.fileIn,cfg.jpegPredictor==9?cfg.blockSize:0)){
        std::cout<<"Invalid Y4M File!"<<std::endl;
        return;
    }

	in.print_header();

	bitstream bss(cfg.fileOut.c_str(),std::ios::trunc|std::ios::binary|std::ios::out);	
	bitstream_wrapper bs(bss,true);	

	mat_golomb_bitstream gb_y(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	mat_golomb_bitstream gb_u(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);
	mat_golomb_bitstream gb_v(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs);

	cv::Size block_size_y,block_size_uv;
	if(cfg.jpegPredictor==9){
		block_size_y=in.get_bsize_y();
		block_size_uv=in.get_bsize_uv();
	}
	else{
		block_size_y=in.get_y_size();
		block_size_uv=in.get_uv_size();
	}
	predictor pd_y(block_size_y.width,block_size_y.height);
	predictor pd_u(block_size_uv.width,block_size_uv.height);
	predictor pd_v(block_size_uv.width,block_size_uv.height);
	
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
	uint total_count = 0;
	//Add Data
	do{
		cv::Mat y=in.get_y(); 
		cv::Mat u=in.get_u();
		cv::Mat v=in.get_v();

		pd_y.newFrame(&y);
		pd_u.newFrame(&u);
		pd_v.newFrame(&v);

		if(cfg.jpegPredictor==9){
			for(uint by=0;by*cfg.blockSize<(uint)y.rows;by++){
				for(uint bx=0;bx*cfg.blockSize<(uint)y.cols;bx++){
					uint bw_y=in.get_bsize_y().width;
					uint bh_y=in.get_bsize_y().height;
					uint bw_uv=in.get_bsize_uv().width;
					uint bh_uv=in.get_bsize_uv().height;

					uint8_t best_pred=std::get<0>(pd_y.calcBestResiduals(bx*bw_y,by*bh_y,&res_y));
					pd_u.calcBlockResiduals(bx*bw_uv,by*bh_uv,best_pred,&res_u);
					pd_v.calcBlockResiduals(bx*bw_uv,by*bh_uv,best_pred,&res_v);

					bs.writeNBits(best_pred,4);	
					gb_y.write_mat(res_y,true);	
					gb_u.write_mat(res_u,true);	
					gb_v.write_mat(res_v,true);	
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

		std::cout << "Processed frame " << ++total_count << "/" << in.get_num_frames() << "\r" << std::flush;
	}while(in.next_frame());
	std::cout << std::endl;	
}

int main(int argc,char** argv){
	args cfg = args(argc, argv, 1);
	encode(cfg);
}
