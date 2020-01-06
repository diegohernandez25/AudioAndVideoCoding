#include "../lib/args/args.h"
#include "../lib/bitstream/bitstream.h"
#include "../lib/bitstream/bitstream_wrapper.h"
#include "../lib/predictor/predictor.h"
#include "../lib/predictor/compensator.h"
#include "../lib/golomb/mat_golomb_bitstream.h"
#include "../lib/y4m/y4m.h"
#include "../lib/dct/dct.h"

class enc_lossy{

	public:
		enc_lossy(args& cfg);
		void encode();

	private:

		const char* magic="VMAD2";

		//Parameters
		uint golomb_initial_m=5;
		uint golomb_blk_size=128;
		uint golomb_calc_interval=16;

		uint compensator_lazy_score=0;

		uint macroblock_threshold=94; //TODO add to args?

		//Members
		args& cfg;

		y4m in;
		bitstream bss;
		bitstream_wrapper bs;

		mat_golomb_bitstream gb_y;
		mat_golomb_bitstream gb_u;
		mat_golomb_bitstream gb_v;

		golomb_bitstream gb_y_rle;
		golomb_bitstream gb_u_rle;
		golomb_bitstream gb_v_rle;
		golomb_bitstream gb_y_rle_zeros;
		golomb_bitstream gb_u_rle_zeros;
		golomb_bitstream gb_v_rle_zeros;

		predictor pd_y;
		predictor pd_u;
		predictor pd_v;

		dct dct_y;
		dct dct_u;
		dct dct_v;

		vector<tuple<short,short>> rle_y, rle_u, rle_v;

		compensator cp;
		boost::circular_buffer<cv::Mat> hist_y,hist_u,hist_v;

		cv::Mat res_y,res_macro_y;
		cv::Mat res_u,res_macro_u;
		cv::Mat res_v,res_macro_v;

		//Functions
		void i_frame();
		void p_frame(cv::Mat& y,cv::Mat& u,cv::Mat& v);
		void write_macroblock(uint mbx,uint mby,cv::Vec4w mvec,cv::Mat& y,cv::Mat& u,cv::Mat& v);
		void applyBlockQuant(uint bx, uint by, uint bw_y, uint bh_y, uint bw_uv, uint bh_uv, uint pred);
		void write_block(uint bx,uint by);

};
