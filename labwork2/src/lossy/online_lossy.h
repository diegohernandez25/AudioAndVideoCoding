#ifndef H_ONLINE_LOSSY
#define H_ONLINE_LOSSY

#include "../lib/bitstream/bitstream.h"
#include "../lib/bitstream/bitstream_wrapper.h"
#include "../lib/golomb/golomb_bitstream.h"
#include "../lib/predictor/predictor.h"
#include "../lib/wav/wav.h"
#include "../lib/quantization/quant.h"

class online_lossy{
	public:
		online_lossy(string& in_file,string& out_file,bool write);
		uint encode();	
		int decode();	

		void set_nr_quant(uint nq);
		void set_pred_order(uint order);

        void set_window_size(uint ws);
        void set_m_calc_int(uint mci);
        void set_initial_m(uint m);

        void set_y_window_size(uint ws);
        void set_y_m_calc_int(uint mci);
        void set_y_initial_m(uint m);

private:
		const char *magic = "CAVN+";
		string &ins;
		string &outs;
		bool write;

		uint nr_quant;
		uint pred_order;

        uint initial_m;
        uint window_size;
        uint m_calc_int;

        uint y_initial_m;
        uint y_window_size;
        uint y_m_calc_int;

};

#endif
