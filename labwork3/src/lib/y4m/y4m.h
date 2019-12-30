#include <fstream>
#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"

class y4m{
	public:
		//Constant values
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

		//IO
		void print_header();
		bool load(std::string &y4mfile,uint block_size=0);
		void save(std::string &y4mfile,uint block_size=0);
		//FIXME need to add way toput sizeon save

		//Creation
		void init(uint width,uint height,uint8_t color_space);
		void set_framerate(uint d,uint n);	
		void set_aspect(uint d,uint n);	
		void set_interlace(uint8_t intr);	
		bool push_frame(cv::Mat &y,cv::Mat &u,cv::Mat &v);

		//Reading 
		uint8_t get_interlace();	
		float get_fps();
		uint* get_framerate();
		uint* get_aspect();
		uint get_width();
		uint get_height();
		uint get_block_size();
		uint8_t get_color_space();
		bool next_frame();
		bool seek(uint frame_number);
		uint get_num_frames();
		cv::Mat get_bgr();
		cv::Mat& get_y();
		cv::Mat& get_u();
		cv::Mat& get_v();

	private:
		//Magic numbers
		const std::string magic{"YUV4MPEG2 "};
		const std::string frame_start{"FRAME"};

		//IO
		std::fstream inf,outf;

		//Parameters
		uint width,height;
		uint fps[2];
		uint8_t interlacing;
		uint pix_asp[2];
		uint8_t color_space;	
		uint block_size;
		
		//Frames
		std::vector<cv::Mat> v_y,v_u,v_v;
		uint frame_ptr;
		cv::Size s_y,s_u,s_v;
		
		//Support functions
		void build_structure();
		bool load_header();
		void save_header();
		bool fetch();
		void store_all();

};
