#include "lossless/online_lossless.h"
#include "lossless/offline_lossless.h"
#include "lossy/online_lossy.h"
#include <iostream>

void online_ly(uint initial_m,uint ws,uint mci,uint y_initial_m,uint y_ws,uint y_mci,uint nr_quant,uint pred_order){
	string orig("test.wav");
	string out("out.mad");
	string back("test_res.wav");


	online_lossy enc_ol(orig,out);	
	enc_ol.set_pred_order(pred_order);

	enc_ol.set_window_size(ws);
	enc_ol.set_m_calc_int(mci);
	enc_ol.set_initial_m(initial_m);

	enc_ol.set_y_window_size(y_ws);
	enc_ol.set_y_m_calc_int(y_mci);
	enc_ol.set_y_initial_m(y_initial_m);

	enc_ol.set_nr_quant(nr_quant);
	enc_ol.encode();

	cout<<"Ended encoding"<<endl;

	online_lossy dec_ol(out,back);	
	if(dec_ol.decode()!=0)
		cout<<"Deu merda."<<endl;
}

void online_less(uint initial_m,uint ws,uint mci,uint y_initial_m,uint y_ws,uint y_mci,uint pred_order){
	string orig("test.wav");
	string out("out.mad");
	string back("test_res.wav");


	online_lossless enc_ol(orig,out);	
	enc_ol.set_pred_order(pred_order);
	
	enc_ol.set_window_size(ws);
	enc_ol.set_m_calc_int(mci);
	enc_ol.set_initial_m(initial_m);

	enc_ol.set_y_window_size(y_ws);
	enc_ol.set_y_m_calc_int(y_mci);
	enc_ol.set_y_initial_m(y_initial_m);

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
	enc_of.set_window_size(44100*7);
	enc_of.set_m_calc_int(44100*7-1);
	enc_of.encode();

	cout<<"Ended encoding"<<endl;

	offline_lossless dec_of(out,back);	
	if(dec_of.decode()!=0)
		cout<<"Deu merda."<<endl;
}

int main(int argc,char** argv){
	if(argc!=9){
		cout<<"Give args"<<endl;
		return 1;
	}
	//online_ly();
	//online_less(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]),atoi(argv[7]));
	online_ly(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]),atoi(argv[7]),atoi(argv[8]));

	//offline_less();
}
