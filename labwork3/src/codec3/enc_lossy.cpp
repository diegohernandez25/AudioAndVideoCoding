#include "enc_lossy.h"

//JPEG pred (0-7 Normal JPEG Preditors, 8 JPEG-LS, 9 Auto prediction)

enc_lossy::enc_lossy(args& cfg):
	cfg(cfg),
	bss(cfg.fileOut.c_str(),std::ios::trunc|std::ios::binary|std::ios::out),
	bs(bss,true),
	gb_y(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs),
	gb_u(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs),
	gb_v(golomb_initial_m,golomb_blk_size,golomb_calc_interval,bs){}

void enc_lossy::encode(){
	in.load(cfg.fileIn,cfg.blockSize);
	in.print_header();

	pd_y=predictor(in.get_bsize_y().width,in.get_bsize_y().height);
	pd_u=predictor(in.get_bsize_uv().width,in.get_bsize_uv().height);
	pd_v=predictor(in.get_bsize_uv().width,in.get_bsize_uv().height);

	cp=compensator(cfg.macroSize*cfg.blockSize,cfg.searchArea,compensator_lazy_score); //TODO add to ARGS search_depth and lazy_score
	hist_y=boost::circular_buffer<cv::Mat>(cfg.searchDepth);
	hist_u=boost::circular_buffer<cv::Mat>(cfg.searchDepth);
	hist_v=boost::circular_buffer<cv::Mat>(cfg.searchDepth);
	
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

	//Write block size
	bs.writeNBits(cfg.blockSize,sizeof(uint)*8);

	//Write macroblock size
	bs.writeNBits(cfg.macroSize,sizeof(uint)*8);

	//Write search depth
	bs.writeNBits(cfg.searchDepth,sizeof(uint16_t)*8);

	//Write lossy mode
	bs.writeBit(cfg.dct?1:0);
	if(cfg.dct) {
		// TODO dct params?
	} else {
		//Write quant bits for each channel
		bs.writeNBits(cfg.qualY, sizeof(short)*8);
		bs.writeNBits(cfg.qualU, sizeof(short)*8);
		bs.writeNBits(cfg.qualV, sizeof(short)*8);
	}

	res_y=cv::Mat(in.get_bsize_y(),CV_16S);
	res_u=cv::Mat(in.get_bsize_uv(),CV_16S);
	res_v=cv::Mat(in.get_bsize_uv(),CV_16S);

	bool first_frame=true;
	uint i_frame_count=0;
	uint p_count = 0;
	uint i_count = 0;
	uint total_count = 0;

	//Add Data
	do{
		cv::Mat y=in.get_y(); 
		cv::Mat u=in.get_u();
		cv::Mat v=in.get_v();

		pd_y.newFrame(&y);
		pd_u.newFrame(&u);
		pd_v.newFrame(&v);	

		if(first_frame||(i_frame_count==(uint)cfg.keyPeriodicity&&cfg.keyPeriodicity!=0)){
			i_frame();
			first_frame=false;
			i_frame_count=0;
			i_count++;
		}
		else{
			p_frame(y,u,v);
			i_frame_count++;
			p_count++;
		}

		hist_y.push_back(y);
		hist_u.push_back(u);
		hist_v.push_back(v);


		std::cout << "Processed frame " << ++total_count << "/" << in.get_num_frames() << " || P-frame count: " << p_count 
				  << " || I-frame count: " << i_count <<"\r" << std::flush;
	}while(in.next_frame());
	std::cout << std::endl;
}

void enc_lossy::i_frame(){
	bs.writeBit(1); //Means I-Frame
	for(uint by=0;by*cfg.blockSize<(uint)in.get_y_size().height;by++){
		for(uint bx=0;bx*cfg.blockSize<(uint)in.get_y_size().width;bx++){
			write_block(bx,by);
		}
	}
}

void enc_lossy::p_frame(cv::Mat& y,cv::Mat& u,cv::Mat& v){
	bs.writeBit(0); //Means P-Frame

	cv::Mat matches=cp.find_matches(y,hist_y);
	for(uint mby=0;mby*cfg.blockSize*cfg.macroSize<(uint)y.rows;mby++){
		for(uint mbx=0;mbx*cfg.blockSize*cfg.macroSize<(uint)y.cols;mbx++){
			if(matches.at<cv::Vec4w>(mby,mbx)[0]>macroblock_threshold*cfg.blockSize*cfg.macroSize){ //Code with intra (I-Block)
				bs.writeBit(1); //Means I-Block
				//Iterate through blocks of macroblock
				for(uint by=mby*cfg.macroSize;by<(mby+1)*cfg.macroSize&&by*cfg.blockSize<(uint)y.rows;by++){ 
					for(uint bx=mbx*cfg.macroSize;bx<(mbx+1)*cfg.macroSize&&bx*cfg.blockSize<(uint)y.cols;bx++){ 
						write_block(bx,by);
					}
				}
			}
			else{ //Code with inter (P-Block)
				bs.writeBit(0); //Means P-Block
				write_macroblock(mbx,mby,matches.at<cv::Vec4w>(mby,mbx),y,u,v);
				
			}
		}
	}
}

void enc_lossy::write_macroblock(uint mbx,uint mby,cv::Vec4w mvec,cv::Mat& y,cv::Mat& u,cv::Mat& v){
	uint mbw_y=in.get_bsize_y().width*cfg.macroSize;
	uint mbh_y=in.get_bsize_y().height*cfg.macroSize;
	uint mbw_uv=in.get_bsize_uv().width*cfg.macroSize;
	uint mbh_uv=in.get_bsize_uv().height*cfg.macroSize;

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
	y(cv::Rect_<uint>(mbx*mbw_y,mby*mbh_y,macroy_x,macroy_y)).convertTo(res_macro_y,CV_16S);
	u(cv::Rect_<uint>(mbx*mbw_uv,mby*mbh_uv,macrouv_x,macrouv_y)).convertTo(res_macro_u,CV_16S);
	v(cv::Rect_<uint>(mbx*mbw_uv,mby*mbh_uv,macrouv_x,macrouv_y)).convertTo(res_macro_v,CV_16S);

	bs.writeNBits(mvec[1],sizeof(4); //Frame Nr
	bs.writeNBits(mvec[2],sizeof(ushort)*8); //VecX
	bs.writeNBits(mvec[3],sizeof(ushort)*8); //VecY
	cv::Vec3w rcv_mvec(mvec[1],mvec[2],mvec[3]);

	cp.apply_block_residual(res_macro_y,hist_y,mvec);

	//readjust vectors sizes
	mvec[2]/=mbw_y/mbw_uv;
	mvec[3]/=mbh_y/mbh_uv;
	cp.apply_block_residual(res_macro_u,hist_u,mvec);
	cp.apply_block_residual(res_macro_v,hist_v,mvec);


	//TODO DCT?
	res_macro_y/=(int)cfg.qualY; //Quantization
	res_macro_u/=(int)cfg.qualU; //Quantization
	res_macro_v/=(int)cfg.qualV; //Quantization
	gb_y.write_mat(res_macro_y,true);
	gb_u.write_mat(res_macro_u,true);
	gb_v.write_mat(res_macro_v,true);

	//Feedback
	res_macro_y*=(int)cfg.qualY;
	res_macro_u*=(int)cfg.qualU;
	res_macro_v*=(int)cfg.qualV;
	cv::Mat blky=y(cv::Rect_<uint>(mbx*mbw_y,mby*mbh_y,macroy_x,macroy_y));
	cp.restore_block(blky,res_macro_y,hist_y,rcv_mvec);

	rcv_mvec[1]/=mbw_y/mbw_uv;
	rcv_mvec[2]/=mbh_y/mbh_uv;
    cv::Mat blku=u(cv::Rect_<uint>(mbx*mbw_uv,mby*mbh_uv,macrouv_x,macrouv_y));
    cv::Mat blkv=v(cv::Rect_<uint>(mbx*mbw_uv,mby*mbh_uv,macrouv_x,macrouv_y));
    cp.restore_block(blku,res_macro_u,hist_u,rcv_mvec);
    cp.restore_block(blkv,res_macro_v,hist_v,rcv_mvec);
}

void enc_lossy::applyBlockQuant(uint bx, uint by, uint bw_y, uint bh_y, uint bw_uv, uint bh_uv, uint pred) {
	short quant_res;
	for (uint j = by*bh_y; j < by*bh_y+bh_y; j++) {
		for (uint i = bx*bw_y; i < bx*bw_y+bw_y; i++) {
			quant_res = (int)(pd_y.calcResidual(i, j, pred) / cfg.qualY);
			res_y.at<short>(j-by*bh_y, i-bx*bw_y) = quant_res;
			pd_y.reconstruct(i, j, pred, quant_res*cfg.qualY);
		}
	}

	for (uint j = by*bh_uv; j < by*bh_uv+bh_uv; j++) {
		for (uint i = bx*bw_uv; i < bx*bw_uv+bw_uv; i++) {
			quant_res = (int)(pd_u.calcResidual(i, j, pred) / cfg.qualU);
			res_u.at<short>(j-by*bh_uv, i-bx*bw_uv) = quant_res;
			pd_u.reconstruct(i, j, pred, quant_res*cfg.qualU);
			quant_res = (int)(pd_v.calcResidual(i, j, pred) / cfg.qualV);
			res_v.at<short>(j-by*bh_uv, i-bx*bw_uv) = quant_res;
			pd_v.reconstruct(i, j, pred, quant_res*cfg.qualV);
		}
	}
}

void enc_lossy::write_block(uint bx,uint by){
	uint bw_y=in.get_bsize_y().width;
	uint bh_y=in.get_bsize_y().height;
	uint bw_uv=in.get_bsize_uv().width;
	uint bh_uv=in.get_bsize_uv().height;
	
	uint8_t best_pred;
	if(cfg.jpegPredictor==9){ //NOT forced mode
		best_pred=std::get<0>(pd_y.calcBestResiduals(bx*bw_y,by*bh_y,&res_y));//FIXME do something with score
		//Maybe return tuple and let the other function decide
		bs.writeNBits(best_pred,4);
	}
	else{
		best_pred=cfg.jpegPredictor;
	}
	
	if (cfg.dct) {
		// TODO
	} else {
		applyBlockQuant(bx, by, bw_y, bh_y, bw_uv, bh_uv, best_pred);
	}

	gb_y.write_mat(res_y,true);
	gb_u.write_mat(res_u,true);
	gb_v.write_mat(res_v,true);

}

int main(int argc,char** argv){
	args cfg = args(argc, argv, 3);
	enc_lossy e(cfg);
	e.encode();
}
