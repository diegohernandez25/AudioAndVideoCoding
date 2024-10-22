#include "y4m.h"


//Documentation about the Y4M (YUV4MPEG) format
//can be found on:
//https://wiki.multimedia.cx/index.php?title=YUV4MPEG2
//or on the Linux Man Page (5) of yuv4mpeg:
//$man 5 yuv4mpeg

void y4m::print_header(){
	std::cout<<"Width: "<<width<<" Height: "<<height<<std::endl;
	std::cout<<"Framerate: "<<fps[0]<<":"<<fps[1]<<std::endl;
	std::cout<<"Num of frame: "<<v_y.size()<<std::endl;
	std::cout<<"Duration: "<<v_y.size()*fps[1]/(float)fps[0]<<"s"<<std::endl;
	std::cout<<"Interlacing: ";
	switch(interlacing){
		case IL_PROGRESSIVE:
			std::cout<<"PROGRESSIVE"<<std::endl;
			break;
		case IL_TOP_FIELD_FIRST:
			std::cout<<"TOP FIELD FIRST"<<std::endl;
			break;
		case IL_BOT_FIELD_FIRST:
			std::cout<<"BOTTON FIELD FIRST"<<std::endl;
			break;
		case IL_MIXED:
			std::cout<<"MIXED"<<std::endl;
			break;
		default:
		case IL_UNSET:
			std::cout<<"UNSET"<<std::endl;
			break;
	}
	std::cout<<"Pixel aspect ratio: "<<pix_asp[0]<<":"<<pix_asp[1]<<std::endl;

	std::cout<<"Colour Space: YUV ";
	switch(color_space){
		case CS444:
			std::cout<<"4:4:4"<<std::endl;
			break;
		case CS422:
			std::cout<<"4:2:2"<<std::endl;
			break;
		case CS420:
			std::cout<<"4:2:0"<<std::endl;
			break;
		case CS420JPEG:
			std::cout<<"4:2:0 JPEG"<<std::endl;
			break;
		case CS420PALDV:
			std::cout<<"4:2:0 PALDV"<<std::endl;
			break;
	}
}
bool y4m::load(std::string &y4mfile,uint block_size){
	this->block_y=cv::Size(block_size,block_size);

	inf.open(y4mfile,std::fstream::in | std::fstream::binary);
	if(!load_header()) return false;
	while(fetch()); //Get all frames
	inf.close();
	return true;
}
bool y4m::load_header(){

	std::string header;
	std::getline(inf,header);

	if(magic.compare(header.substr(0,magic.size()))!=0){
		return false;
	}

	//Reset values
	width=0; height=0;
	fps[0]=0; fps[1]=0;
	interlacing=IL_UNSET;
	pix_asp[0]=0; pix_asp[1]=0;
	color_space=CS420JPEG;


	size_t currptr=header.find(" ")+1;
	size_t nextptr;
	std::string val;
	while(true){
		nextptr=header.find(" ",currptr);
		if(nextptr==std::string::npos)
			val=header.substr(currptr+1,nextptr);
		else
			val=header.substr(currptr+1,nextptr-currptr+1);

		size_t sep;
		switch(header[currptr]){
			case 'W': //Width
				width=stoi(val);
				break;
			case 'H': //Height
				height=stoi(val);
				break;
			case 'F': //Frame rate
				sep=val.find(":");
				fps[0]=stoi(val.substr(0,sep));
				fps[1]=stoi(val.substr(sep+1,std::string::npos));
				break;
			case 'A': //Pixel Aspect Ratio
				sep=val.find(":");
				pix_asp[0]=stoi(val.substr(0,sep));
				pix_asp[1]=stoi(val.substr(sep+1,std::string::npos));
				break;
			case 'I': //Interlacing
				switch(val[0]){
					case 'p':
						interlacing=IL_PROGRESSIVE;
						break;
					case 't':
						interlacing=IL_TOP_FIELD_FIRST;
						break;
					case 'b':
						interlacing=IL_BOT_FIELD_FIRST;
						break;
					case 'm':
						interlacing=IL_MIXED;
						break;
					case '?':
						interlacing=IL_UNSET;
						break;
				}
				break;
			case 'C': //Colour space
				if(val.compare("420jpeg")==0)
					color_space=CS420JPEG;
				else if(val.compare("420paldv")==0)
					color_space=CS420PALDV;
				else if(val.compare("420")==0)
					color_space=CS420;
				else if(val.compare("422")==0)
					color_space=CS422;
				else if(val.compare("444")==0)
					color_space=CS444;
				break;
			case 'X': //Comment,ignored
				break;
			default:
				std::cout<<"y4m: Unknown header option..."<<std::endl;
				//return false;
				break;
		}
		if(nextptr==std::string::npos) break;
		currptr=nextptr+1;
	}

	v_y.clear();
	v_u.clear();
	v_v.clear();
	frame_ptr=0;
	build_structure();

	return true;
}

void y4m::save(std::string &y4mfile){
	outf.open(y4mfile,std::fstream::out | std::fstream::binary | std::fstream::trunc);
	save_header();
	store_all();
	outf.close();
}

void y4m::save_header(){
	outf<<magic<<"W"<<width<<" H"<<height
		<<" F"<<fps[0]<<":"<<fps[1]
		<<" A"<<pix_asp[0]<<":"<<pix_asp[1]
		<<" I";

	if(interlacing==IL_PROGRESSIVE)
		outf<<"p";
	else if(interlacing==IL_TOP_FIELD_FIRST)
		outf<<"t";
	else if(interlacing==IL_BOT_FIELD_FIRST)
		outf<<"b";
	else if(interlacing==IL_MIXED)
		outf<<"m";
	else /*if(interlacing==IL_UNSET)*/
		outf<<"?";

	outf<<" C";
	if(color_space==CS444)
		outf<<"444";
	else if(color_space==CS422)
		outf<<"422";
	else if(color_space==CS420)
		outf<<"420";
	else if(color_space==CS420JPEG)
		outf<<"420jpeg";
	else /*if(color_space==CS420PALDV)*/
		outf<<"420paldv";

	outf<<std::endl;
}

void y4m::store_all(){
	for(uint i=0;i<v_y.size();i++){
		//Magic
		outf<<frame_start;
		outf<<std::endl;

		//Crop Padding
		cv::Mat y_clone=v_y[i](cv::Rect(0,0,s_y.width,s_y.height)).clone();
		cv::Mat u_clone=v_u[i](cv::Rect(0,0,s_uv.width,s_uv.height)).clone();
		cv::Mat v_clone=v_v[i](cv::Rect(0,0,s_uv.width,s_uv.height)).clone();

		//Store y
		outf.write((char*) y_clone.ptr(),s_y.height*s_y.width);

		//Store u
		outf.write((char*) u_clone.ptr(),s_uv.height*s_uv.width);

		//Store v
		outf.write((char*) v_clone.ptr(),s_uv.height*s_uv.width);
	}
}

void y4m::build_structure(){
	s_y=cv::Size(width,height);

	if(block_y.width>0){
		s_y_pad=cv::Size(block_y.width*ceil((float)width/block_y.width),block_y.height*ceil((float)height/block_y.height));
	}
	else
		s_y_pad=s_y;

	switch(color_space){
		case CS444:
			s_uv=s_y;
			block_uv=block_y;
			s_uv_pad=s_y_pad;
			break;
		case CS422:
			s_uv=cv::Size((width+1)/2,height);
			block_uv=cv::Size(block_y.width/2,block_y.height);
			s_uv_pad=cv::Size(s_y_pad.width/2,s_y_pad.height);
			break;
		case CS420:
		case CS420JPEG:
		case CS420PALDV:
			s_uv=cv::Size((width+1)/2,(height+1)/2);
			block_uv=cv::Size(block_y.width/2,block_y.height/2);
			s_uv_pad=cv::Size(s_y_pad.width/2,s_y_pad.height/2);
			break;
	}
}


bool y4m::fetch(){
	cv::Mat y(s_y,CV_8U);
	cv::Mat u(s_uv,CV_8U);
	cv::Mat v(s_uv,CV_8U);

	std::string frame_header;
	std::getline(inf,frame_header);

	if(frame_start.compare(frame_header.substr(0,frame_start.size()))!=0){
		return false;
	}

	size_t currptr=frame_header.find(" ");
	size_t nextptr;
	std::string val;
	while(currptr!=std::string::npos){
		currptr++;
		nextptr=frame_header.find(" ",currptr);
		if(nextptr==std::string::npos)
			val=frame_header.substr(currptr+1,nextptr);
		else
			val=frame_header.substr(currptr+1,nextptr-currptr+1);

	}

	//From OpenCV documentation:
	//"Matrices created with Mat::create are always continuous."
	//This means one can store and read directly using pointer arithmetic

	//Read y
	inf.read((char*) y.ptr(),y.rows*y.cols);

	//Read u
	inf.read((char*) u.ptr(),u.rows*u.cols);

	//Read v
	inf.read((char*) v.ptr(),v.rows*v.cols);

	//Pad, if needed
	if(block_y.width>1){
		uint pad_w_y=s_y_pad.width-s_y.width;
		uint pad_h_y=s_y_pad.height-s_y.height;
		uint pad_w_uv=s_uv_pad.width-s_uv.width;
		uint pad_h_uv=s_uv_pad.height-s_uv.height;

		cv::copyMakeBorder(y,y,0,pad_h_y,0,pad_w_y,cv::BORDER_REFLECT);
		cv::copyMakeBorder(u,u,0,pad_h_uv,0,pad_w_uv,cv::BORDER_REFLECT);
		cv::copyMakeBorder(v,v,0,pad_h_uv,0,pad_w_uv,cv::BORDER_REFLECT);
	}

	v_y.push_back(y);
	v_u.push_back(u);
	v_v.push_back(v);

	return true;
}

void y4m::init(uint width,uint height,uint8_t color_space,uint block_size){
    this->block_y=cv::Size(block_size,block_size);
	this->width=width;
	this->height=height;
	this->color_space=color_space;
    fps[0]=0; fps[1]=0;
    interlacing=IL_UNSET;
    pix_asp[0]=0; pix_asp[1]=0;

	v_y.clear();
	v_u.clear();
	v_v.clear();
	frame_ptr=0;
	build_structure();
}

void y4m::set_framerate(uint d,uint n){
	fps[0]=d;
	fps[1]=n;
}

void y4m::set_aspect(uint d,uint n){
	pix_asp[0]=d;
	pix_asp[1]=n;
}

void y4m::set_interlace(uint8_t intr){ interlacing=intr; }

bool y4m::push_frame(cv::Mat &y,cv::Mat &u,cv::Mat &v){
	if(y.rows!=s_y_pad.height||y.cols!=s_y_pad.width||
	   u.rows!=s_uv_pad.height||u.cols!=s_uv_pad.width||
	   v.rows!=s_uv_pad.height||v.cols!=s_uv_pad.width)
		return false;

	v_y.push_back(y);
	v_u.push_back(u);
	v_v.push_back(v);
	return true;
}

bool y4m::seek(uint frame_number){
	frame_ptr=frame_number;
	return frame_ptr<v_y.size();
}
bool y4m::next_frame(){
	frame_ptr++;
	return frame_ptr<v_y.size();
}

cv::Mat y4m::get_bgr(){
	cv::Mat fy=v_y[frame_ptr];
	cv::Mat adj_u,adj_v;
	cv::Mat yuv,bgr;
	cv::resize(v_u[frame_ptr],adj_u,fy.size(),0,0,cv::INTER_LINEAR);
	cv::resize(v_v[frame_ptr],adj_v,fy.size(),0,0,cv::INTER_LINEAR);
	cv::Mat chan[3]={fy,adj_u,adj_v};
	cv::merge(chan,3,yuv);
	cv::cvtColor(yuv,bgr,cv::COLOR_YUV2BGR);
	return bgr;
}

uint y4m::get_width(){ return width; }
uint y4m::get_height(){ return height; }
uint8_t y4m::get_color_space(){ return color_space; }
uint8_t y4m::get_interlace(){ return interlacing; }
float y4m::get_fps(){ return fps[0]/fps[1]; }
uint* y4m::get_framerate(){ return fps; }
uint* y4m::get_aspect(){ return pix_asp; }
uint y4m::get_num_frames(){ return v_y.size(); }
cv::Mat& y4m::get_y(){ return v_y[frame_ptr]; }
cv::Mat& y4m::get_u(){ return v_u[frame_ptr]; }
cv::Mat& y4m::get_v(){ return v_v[frame_ptr]; }
cv::Size y4m::get_bsize_y(){ return block_y; }
cv::Size y4m::get_bsize_uv(){ return block_uv; }
cv::Size y4m::get_y_size(){ return s_y; }
cv::Size y4m::get_uv_size(){ return s_uv; }
cv::Size y4m::get_pady_size(){ return s_y_pad; }
cv::Size y4m::get_paduv_size(){ return s_uv_pad; }
