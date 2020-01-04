#include "../lib/args/args.h"
#include "../lib/bitstream/bitstream.h"
#include "../lib/bitstream/bitstream_wrapper.h"
#include "../lib/predictor/predictor.h"
#include "../lib/predictor/compensator.h"
#include "../lib/golomb/mat_golomb_bitstream.h"
#include "../lib/y4m/y4m.h"

class dec_hybrid{

	public:
		dec_hybrid(args& cfg);
		int decode();

	private:

		const char* magic="VMAD1";

		//Parameters
		uint golomb_initial_m=5;
		uint golomb_blk_size=128;
		uint golomb_calc_interval=16;

		uint compensator_depth;
		uint compensator_lazy_score;


		//Members	
		args& cfg;

		uint pred_mode;
		uint predBlockSize;
		uint macroSize;

		y4m out;
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
		void read_macroblock(uint mbx,uint mby,cv::Mat& y,cv::Mat& u,cv::Mat& v);
		void read_block(uint bx,uint by);

};
