#include "dec_lossy.h"

dec_lossy::dec_lossy(args& cfg):
    cfg(cfg),
    bss(cfg.fileIn.c_str(),std::ios::binary|std::ios::in),
    bs(bss,true),
    gb_y(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs),
    gb_u(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs),
    gb_v(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs){}


int dec_lossy::decode(){

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
	pred_mode=bs.readNBits(sizeof(uint8_t)*8);

	//Read block size
	predBlockSize=bs.readNBits(sizeof(uint)*8);

	//Read macroblock size
	macroSize=bs.readNBits(sizeof(uint)*8);

	//Read search depth
	uint compensator_depth=bs.readNBits(sizeof(uint16_t)*8);

	//Read lossy mode
	useDct = bs.readBit();
	if (useDct) {
		// TODO dct params?
	} else {
		qualY = bs.readNBits(sizeof(short)*8);
		qualU = bs.readNBits(sizeof(short)*8);
		qualV = bs.readNBits(sizeof(short)*8);
	}

	//Create new video
	out.init(width,height,color_space,predBlockSize);
	out.set_framerate(framerate[0],framerate[1]);
	out.set_aspect(aspect[0],aspect[1]);
	out.set_interlace(interlace);
	out.print_header();

	cp=compensator(predBlockSize*macroSize,0,0);
    hist_y=boost::circular_buffer<cv::Mat>(compensator_depth);
    hist_u=boost::circular_buffer<cv::Mat>(compensator_depth);
    hist_v=boost::circular_buffer<cv::Mat>(compensator_depth);

	pd_y=predictor(out.get_bsize_y().width,out.get_bsize_y().height);
	pd_u=predictor(out.get_bsize_uv().width,out.get_bsize_uv().height);
	pd_v=predictor(out.get_bsize_uv().width,out.get_bsize_uv().height);

	res_y=cv::Mat(out.get_bsize_y(),CV_16S);
    res_u=cv::Mat(out.get_bsize_uv(),CV_16S);
    res_v=cv::Mat(out.get_bsize_uv(),CV_16S);

	//Get Data
	for(uint f=0;f<num_frames;f++){
		cv::Mat y(out.get_pady_size(),CV_8U);
		cv::Mat u(out.get_paduv_size(),CV_8U);
		cv::Mat v(out.get_paduv_size(),CV_8U);

		pd_y.newFrame(&y);
		pd_u.newFrame(&u);
		pd_v.newFrame(&v);

		bool iframe=bs.readBit();

		if(iframe)
			i_frame();
		else{
			p_frame(y,u,v);
		}

		hist_y.push_back(y);
		hist_u.push_back(u);
		hist_v.push_back(v);

		out.push_frame(y,u,v);
		std::cout << "Processed frame " << f+1 << "/" << num_frames << "\r" << std::flush;
	}
	out.save(cfg.fileOut);
	std::cout << std::endl;
	return 0;
}

void dec_lossy::i_frame(){
	for(uint by=0;by*predBlockSize<(uint)out.get_y_size().height;by++){
		for(uint bx=0;bx*predBlockSize<(uint)out.get_y_size().width;bx++){
			read_block(bx,by);
		}
	}
}

void dec_lossy::p_frame(cv::Mat& y,cv::Mat& u,cv::Mat& v){
    for(uint mby=0;mby*predBlockSize*macroSize<(uint)y.rows;mby++){
        for(uint mbx=0;mbx*predBlockSize*macroSize<(uint)y.cols;mbx++){
			bool iblock=bs.readBit();

            if(iblock){ //Decode with intra (I-Block)
                //Iterate through blocks of macroblock
                for(uint by=mby*macroSize;by<(mby+1)*macroSize&&by*predBlockSize<(uint)y.rows;by++){
                    for(uint bx=mbx*macroSize;bx<(mbx+1)*macroSize&&bx*predBlockSize<(uint)y.cols;bx++){

                        read_block(bx,by);
                    }
                }
            }
            else{ //Decode with inter (P-Block)
                read_macroblock(mbx,mby,y,u,v);
            }
        }
    }
}

void dec_lossy::read_macroblock(uint mbx,uint mby,cv::Mat& y,cv::Mat& u,cv::Mat& v){
	uint framenr=bs.readNBits(sizeof(ushort)*8); //Frame Nr
	uint vecx=bs.readNBits(sizeof(ushort)*8); //VecX
	uint vecy=bs.readNBits(sizeof(ushort)*8); //VecY
	cv::Vec3w mvec(framenr,vecx,vecy);

    uint mbw_y=out.get_bsize_y().width*macroSize;
    uint mbh_y=out.get_bsize_y().height*macroSize;
    uint mbw_uv=out.get_bsize_uv().width*macroSize;
    uint mbh_uv=out.get_bsize_uv().height*macroSize;

    uint macroy_x=mbw_y;
    uint macroy_y=mbh_y;
    uint macrouv_x=mbw_uv;
    uint macrouv_y=mbh_uv;
    if(mbx*mbw_y+mbw_y>(uint)y.cols){
        macroy_x=y.cols-mbx*mbw_y;
        macrouv_x=u.cols-mbx*mbw_uv;
    }
    if(mby*mbh_y+mbh_y>(uint)y.rows){
        macroy_y=y.rows-mby*mbh_y;
        macrouv_y=u.rows-mby*mbh_uv;
    }

	res_macro_y=cv::Mat(macroy_y,macroy_x,CV_16S);	
	res_macro_u=cv::Mat(macrouv_y,macrouv_x,CV_16S);	
	res_macro_v=cv::Mat(macrouv_y,macrouv_x,CV_16S);	

    gb_y.read_mat(res_macro_y,true);
    gb_u.read_mat(res_macro_u,true);
    gb_v.read_mat(res_macro_v,true);
	res_macro_y*=qualY; //De-Quantization //TODO maybe use another params?
    res_macro_u*=qualU; //De-Quantization
    res_macro_v*=qualV; //De-Quantization

	cv::Mat blky=y(cv::Rect_<uint>(mbx*mbw_y,mby*mbh_y,macroy_x,macroy_y));
	cv::Mat blku=u(cv::Rect_<uint>(mbx*mbw_uv,mby*mbh_uv,macrouv_x,macrouv_y));
	cv::Mat blkv=v(cv::Rect_<uint>(mbx*mbw_uv,mby*mbh_uv,macrouv_x,macrouv_y));

    cp.restore_block(blky,res_macro_y,hist_y,mvec);

    //readjust vectors sizes
    mvec[1]/=mbw_y/mbw_uv;
    mvec[2]/=mbh_y/mbh_uv;

	cp.restore_block(blku,res_macro_u,hist_u,mvec);
	cp.restore_block(blkv,res_macro_v,hist_v,mvec);
}

void dec_lossy::read_block(uint bx,uint by){
	uint bw_y=out.get_bsize_y().width;
	uint bh_y=out.get_bsize_y().height;
	uint bw_uv=out.get_bsize_uv().width;
	uint bh_uv=out.get_bsize_uv().height;

	uint8_t best_pred;
	if(pred_mode==9){ //NOT forced mode
		best_pred=bs.readNBits(4);
	}
	else{
		best_pred=pred_mode;
	}

	gb_y.read_mat(res_y,true);
	gb_u.read_mat(res_u,true);
	gb_v.read_mat(res_v,true);

	if(useDct) {
		//TODO
	} else {
		res_y *= (int)qualY;
		res_u *= (int)qualU;
		res_v *= (int)qualV;
	}

	pd_y.reconstructBlock(bx*bw_y,by*bh_y,best_pred,&res_y);
	pd_u.reconstructBlock(bx*bw_uv,by*bh_uv,best_pred,&res_u);
	pd_v.reconstructBlock(bx*bw_uv,by*bh_uv,best_pred,&res_v);
}

int main(int argc,char** argv){
	args cfg = args(argc, argv, 0);
    dec_lossy d(cfg);
	return d.decode();
}
