#ifndef H_GOLOMB_BITSTREAM
#define H_GOLOMB_BITSTREAM

#include <tuple>
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>

#include "../bitstream/bitstream.h"
#include "golomb.h"

class golomb_bitstream : public golomb{
	public:
		golomb_bitstream(uint initial_m,uint blk_size,uint calc_interval,bitstream& bt);
		golomb_bitstream(uint m,bitstream& bt);

		void write_signed_val(int val);
		int read_signed_val();

		void write_val(uint val);
		uint read_val();

	private:
		bool signed_vals;
		bitstream& bt;
};

#endif
