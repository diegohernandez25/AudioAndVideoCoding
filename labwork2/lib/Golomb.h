#include <tuple>
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>

class Golomb{
	public:
		Golomb(uint initial_m,uint blk_size,uint calc_interval);	
		Golomb(uint m);	
		std::tuple<uint,uint> encode(uint n);
		uint decode(uint q,uint r);
		std::tuple<uint,uint> signed_encode(int n);
		int signed_decode(uint q,uint r);

	private:
		uint m;
		uint blk_size,blk_ptr;
		uint calc_interval,calc_counter;
		std::vector<uint> block;
		bool buffer_full;

		void predict_m();
		void adjust_golomb(uint n);
};
