#ifndef H_GOLOMB
#define H_GOLOMB

#include <tuple>
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>

class golomb{
	public:
		golomb(){}	
		golomb(uint m);	
		golomb(uint initial_m,uint blk_size,uint calc_interval);	

		std::tuple<uint,uint> encode(uint n);
		uint decode(uint q,uint r);
		std::tuple<uint,uint> signed_encode(int n);
		int signed_decode(uint q,uint r);
		int get_m();
		void set_m(int new_m);

	private:
		uint m;
		uint blk_size,blk_ptr;
		uint calc_interval,calc_counter;
		std::vector<uint> block;
		bool buffer_full;
		uint curr_acc;

		void predict_m();
		void recalc_acc(uint new_n);
		void adjust_golomb(uint n);
};

#endif
