#include "../lib/args/args.h"
#include "../lib/bitstream/bitstream.h"
#include "../lib/bitstream/bitstream_wrapper.h"
#include "../lib/predictor/predictor.h"
#include "../lib/predictor/compensator.h"
#include "../lib/golomb/mat_golomb_bitstream.h"
#include "../lib/y4m/y4m.h"

class enc_hybrid{

	public:
		enc_hybrid(args& cfg);
		void encode();

	private:

		const char* magic="VMAD1";

		//Parameters
		uint golomb_initial_m=5;
		uint golomb_blk_size=128;
		uint golomb_calc_interval=16;

		uint compensator_depth=4;
		uint compensator_lazy_score=0;

		uint macroblock_threshold=1000;//FIXME

		//Members	
		args& cfg;

		y4m in;
		bitstream bss;
		bitstream_wrapper bs;

		mat_golomb_bitstream gb_y;
		mat_golomb_bitstream gb_u;
		mat_golomb_bitstream gb_v;

		predictor pd_y;
		predictor pd_u;
		predictor pd_v;

		compensator cp; 
		boost::circular_buffer<cv::Mat> hist_y,hist_u,hist_v;

		cv::Mat res_y,res_macro_y;
		cv::Mat res_u,res_macro_u;
		cv::Mat res_v,res_macro_v;

		//Functions
		void i_frame();
		void p_frame(cv::Mat& y,cv::Mat& u,cv::Mat& v);
		void write_macroblock(uint mbx,uint mby,cv::Vec4w mvec,cv::Mat& y,cv::Mat& u,cv::Mat& v);
		void write_block(uint bx,uint by);

};