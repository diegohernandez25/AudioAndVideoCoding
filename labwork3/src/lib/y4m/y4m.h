#include<fstream>
#include<iostream>
#include<string>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"

class y4m{
	public:
		static const uint IL_PROGRESSIVE=0; //IL = Interlacing
		static const uint IL_TOP_FIELD_FIRST=1;
		static const uint IL_BOT_FIELD_FIRST=2;
		static const uint IL_MIXED=3;
		static const uint IL_UNSET=4;

		static const uint CS444=0; //CS = Colour Space
		static const uint CS422=1;
		static const uint CS420=2;
		static const uint CS420JPEG=3;
		static const uint CS420PALDV=4;

		void print_header();
		bool load(std::string &y4mfile);

		bool next_frame();

		cv::Mat get_bgr();
		cv::Mat& get_y();
		cv::Mat& get_u();
		cv::Mat& get_v();

	private:
		const std::string magic{"YUV4MPEG2 "};
		const std::string frame_start{"FRAME"};

		std::fstream inf,outf;

		//Parameters
		uint width,height;
		uint fps[2];
		uint8_t interlacing;
		uint pix_asp[2];
		uint8_t color_space;	
		
		//Mats	
		cv::Mat y,u,v;

};
