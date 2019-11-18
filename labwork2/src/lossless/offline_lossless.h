#ifndef H_OFFLINE_LOSSLESS
#define H_OFFLINE_LOSSLESS

#include "../lib/bitstream/bitstream.h"
#include "../lib/golomb/golomb_bitstream.h"
#include "../lib/predictor/predictor.h"
#include "../lib/wav/wav.h"

class offline_lossless{
	public:
		offline_lossless(string& in_file,string& out_file);
		void encode();	
		int decode();	

		void set_window_size(uint ws);
		void set_m_calc_int(uint m);

	private:
		const char* magic = "CAVF+";
		string &ins;
		string &outs;

		uint window_size;
		uint m_calc_int;
		
};

#endif
