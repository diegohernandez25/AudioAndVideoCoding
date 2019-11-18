#include "lossless/online_lossless.h"
#include "lossless/offline_lossless.h"
#include "lossy/online_lossy.h"
#include <iostream>

void online_ly(){
	string orig("test.wav");
	string out("out.mad");
	string back("test_res.wav");


	online_lossy enc_ol(orig,out);	
	enc_ol.set_window_size(40000);
	enc_ol.set_m_calc_int(20000);
	enc_ol.set_initial_m(1<<(8*sizeof(short)-1));
	enc_ol.set_nr_quant(0);
	enc_ol.encode();

	cout<<"Ended encoding"<<endl;

	online_lossy dec_ol(out,back);	
	if(dec_ol.decode()!=0)
		cout<<"Deu merda."<<endl;
}

void online_less(){
	string orig("test.wav");
	string out("out.mad");
	string back("test_res.wav");


	online_lossless enc_ol(orig,out);	
	enc_ol.set_window_size(440);
	enc_ol.set_m_calc_int(0);
	enc_ol.set_initial_m(1<<(8*sizeof(short)-1));
	enc_ol.encode();

	cout<<"Ended encoding"<<endl;

	online_lossless dec_ol(out,back);	
	if(dec_ol.decode()!=0)
		cout<<"Deu merda."<<endl;
}

void offline_less(){
	string orig("test.wav");
	string out("out.mad");
	string back("test_res.wav");


	offline_lossless enc_of(orig,out);	
	enc_of.set_window_size(4400);
	enc_of.set_m_calc_int(4400-1);
	enc_of.encode();

	cout<<"Ended encoding"<<endl;

	offline_lossless dec_of(out,back);	
	if(dec_of.decode()!=0)
		cout<<"Deu merda."<<endl;
}

int main(int argc,char** argv){
	online_ly();
	//online_less();
	//offline_less();
}
