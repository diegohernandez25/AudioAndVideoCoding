#include "golomb.h"

golomb::golomb(uint m) : golomb(m,0,0){}

golomb::golomb(uint initial_m,uint blk_size,uint calc_interval){
	assert(blk_size==0||calc_interval<blk_size);

	m=initial_m;

	this->blk_size=blk_size;
	blk_ptr=0;

	calc_counter=calc_interval;
	this->calc_interval=calc_interval;
	
	buffer_full=false;
	if(blk_size!=0)
		block=std::vector<uint>((std::vector<uint>::size_type) blk_size);
}

std::tuple<uint,uint> golomb::encode(uint n){
	uint q=n/m;
	std::tuple<uint,uint> ret = std::make_tuple(q,n-q*m);
	adjust_golomb(n);
	return ret; 
}

uint golomb::decode(uint q,uint r){
	uint n=q*m+r;
	adjust_golomb(n);
	return n;
} 

void golomb::adjust_golomb(uint n){
	if(blk_size!=0){
		block[blk_ptr++]=n;
		if(!buffer_full&&blk_ptr==blk_size) buffer_full=true;
		blk_ptr%=blk_size;

		if(buffer_full&&(calc_counter++)==calc_interval){
			predict_m();
			calc_counter=0;
		}
	}
}

void golomb::predict_m(){
	//Golomb follows a geometric distribution
	//Geometric distribution mean = (1-p)/p
	//p = (1-alpha), so mean = alpha/(1-alpha)
	//Solving for alpha gives: alpha = mean/(mean+1)
	//Finally m = ceil(-1/log2(alpha))
	double mean=0;
	for(auto it=block.begin();it!=block.end();it++){
		mean+=(double)*it/blk_size;
	}
	if(mean==0) return; //This should never happen, but we need to protect against this case

	m=std::ceil(-1/std::log2(mean/(mean+1.0)));
	//std::cout<<m<<std::endl;
}

std::tuple<uint,uint> golomb::signed_encode(int n){
	//positive (and zero) -> even
	//negative -> odd 
	return encode(n>=0?2*n:-2*n-1);
}

int golomb::signed_decode(uint q,uint r){
	//odd -> negative 
	//even -> positive
	uint res=decode(q,r);
	return res%2 ? ((int)res+1)/-2 : res/2;
}

int golomb::get_m(){return m;}
void golomb::set_m(int new_m){ m=new_m;}
/*
int main(int argc,char** argv){
	golomb genc(20,6,0);
	golomb gdec(20,6,0);
	int a=0,q,r;
	std::tuple<uint,uint> res;
	while(true){
		std::cin>>a;	
		res=genc.signed_encode(a);
		q=std::get<0>(res);
		r=std::get<1>(res);
		std::cout<<"In="<<a<<" q="<<q<<" r="<<r<<" Out="<<gdec.signed_decode(q,r)<<std::endl;
	}
	return 0;
}
*/
