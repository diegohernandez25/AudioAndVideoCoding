#ifndef H_GOLOMB_BITSTREAM
#define H_GOLOMB_BITSTREAM

#include <tuple>
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>

#include "bitstream.h"
#include "golomb.h"

//TODO Maybe faria sentido extender golomb
class golomb_bitstream{
	public:
		golomb_bitstream(uint initial_m,uint blk_size,uint calc_interval,bitstream& bt);
		golomb_bitstream(uint m,bitstream& bt);

		void write_signed_val(int val);
		void write_val(uint val);

		int read_signed_val();
		uint read_val();

	private:
		bool signed_vals;
		golomb gl;
		bitstream& bt;
};

#endif
