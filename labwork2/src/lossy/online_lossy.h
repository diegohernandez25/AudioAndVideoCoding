#ifndef H_ONLINE_LOSSY
#define H_ONLINE_LOSSY

#include "../lib/bitstream/bitstream.h"
#include "../lib/golomb/golomb_bitstream.h"
#include "../lib/predictor/predictor.h"
#include "../lib/wav/wav.h"
#include "../lib/quantization/quant.h"

class online_lossy{
	public:
		online_lossy(string& in_file,string& out_file);
		void encode();	
		int decode();	

		void set_nr_quant(uint nq);
        void set_window_size(uint ws);
        void set_m_calc_int(uint mci);
        void set_initial_m(uint m);

	private:
		const char *magic = "CAVN+";
		string &ins;
		string &outs;

		uint nr_quant;
        uint initial_m;
        uint window_size;
        uint m_calc_int;

};

#endif