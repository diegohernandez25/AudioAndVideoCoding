#include "y4m.h"

void y4m::print_header(){
	std::cout<<"Width: "<<width<<" Height: "<<height<<std::endl;
	std::cout<<"Framerate: "<<fps[0]<<":"<<fps[1]<<std::endl;
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
			std::cout<<"4:4:4 JPEG"<<std::endl;
			break;
		case CS420PALDV:
			std::cout<<"4:4:4 PALDV"<<std::endl;
			break;
	}
}
bool y4m::load(std::string &y4mfile){
	inf.open(y4mfile,std::fstream::in | std::fstream::binary);
	
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
	color_space=CS444;


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
	//Recreate Mats
	y.create(height,width,CV_8U);
	switch(color_space){
		case CS444:
			u.create(height,width,CV_8U);
			v.create(height,width,CV_8U);
			break;
		case CS422:
			u.create(height,(width+1)/2,CV_8U);
			v.create(height,(width+1)/2,CV_8U);
			break;
		case CS420:
		case CS420JPEG:
		case CS420PALDV:
			u.create((height+1)/2,(width+1)/2,CV_8U);
			v.create((height+1)/2,(width+1)/2,CV_8U);
			break;
	}

	return true;
}

bool y4m::next_frame(){
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

		//TODO interpret frame_header specific options?
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

	return true;
}

cv::Mat y4m::get_bgr(){
	cv::Mat adj_u,adj_v;
	cv::Mat yuv,bgr;
	cv::resize(u,adj_u,y.size(),0,0,cv::INTER_LINEAR);
	cv::resize(v,adj_v,y.size(),0,0,cv::INTER_LINEAR);
	cv::Mat chan[3]={y,adj_u,adj_v};
	cv::merge(chan,3,yuv);	
	cv::cvtColor(yuv,bgr,cv::COLOR_YUV2BGR);
	return bgr;
}

cv::Mat& y4m::get_y(){ return y; }
cv::Mat& y4m::get_u(){ return u; }
cv::Mat& y4m::get_v(){ return v; }

#include "opencv2/highgui/highgui.hpp" //FIXME temporary
int main(int argc,char **argv){
	y4m a;
	std::string f(argv[1]);
	a.load(f);
	a.print_header();

	while(a.next_frame()){
		cv::imshow("BGR",a.get_bgr());
		cv::imshow("Y",a.get_y());
		cv::imshow("U",a.get_u());
		cv::imshow("V",a.get_v());
		cv::waitKey(0);
	}
}
