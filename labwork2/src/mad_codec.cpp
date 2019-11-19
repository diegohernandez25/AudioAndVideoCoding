#include "lossless/online_lossless.h"
#include "lossless/offline_lossless.h"
#include "lossy/online_lossy.h"
#include <iostream>

void online_ly(uint initial_m,uint ws,uint mci,uint y_initial_m,uint y_ws,uint y_mci,uint nr_quant,uint pred_order){
	string orig("test.wav");
	string out("out.mad");
	string back("test_res.wav");


	online_lossy enc_ol(orig,out,true);	
	enc_ol.set_pred_order(pred_order);

	enc_ol.set_window_size(ws);
	enc_ol.set_m_calc_int(mci);
	enc_ol.set_initial_m(initial_m);

	enc_ol.set_y_window_size(y_ws);
	enc_ol.set_y_m_calc_int(y_mci);
	enc_ol.set_y_initial_m(y_initial_m);

	enc_ol.set_nr_quant(nr_quant);

	cout<<"Ended encoding "<<enc_ol.encode()<<endl;

	online_lossy dec_ol(out,back,true);	
	if(dec_ol.decode()!=0)
		cout<<"Deu merda."<<endl;
}

void online_less(uint initial_m,uint ws,uint mci,uint y_initial_m,uint y_ws,uint y_mci,uint pred_order){
	string orig("test.wav");
	string out("out.mad");
	string back("test_res.wav");


	online_lossless enc_ol(orig,out,true);	
	enc_ol.set_pred_order(pred_order);
	
	enc_ol.set_window_size(ws);
	enc_ol.set_m_calc_int(mci);
	enc_ol.set_initial_m(initial_m);

	enc_ol.set_y_window_size(y_ws);
	enc_ol.set_y_m_calc_int(y_mci);
	enc_ol.set_y_initial_m(y_initial_m);

	enc_ol.encode();
	
	cout<<"Ended encoding"<<endl;

	online_lossless dec_ol(out,back,true);	
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

int handleFlagParse(int elem, char* next, string shortFlag, string longFlag, string value,
					string flagMeaning, string placeholder, string description) {
	if (elem == 0) {
		cout << "Error: a " << value << " has to be declared after the " 
				<< flagMeaning << " flag '" << shortFlag << "'.\n	"
				<< shortFlag << " OR  " << longFlag << " <" << placeholder 
				<< "> : " << description << endl;
		return -1;
	}
	if (next[0] == '-'){
		cout << "Error: a " << value << " has to be declared after the " 
				<< flagMeaning << " flag '" << shortFlag << "'." << endl
				<< "Another flag '" << next << "' was parsed instead.\n	"
				<< shortFlag << " OR  " << longFlag << " <" << placeholder 
				<< "> : " << description << endl;
		return -1;
	}
	return 0;
}

int parseArgs(int elem, char** argv, int* encode, int* lossy, string* fileIn, string* fileOut,
			  int* windowSize, int* skipNSamples, int* quantBits, int* predictorOrder,
			  int* windowSizeY, int* skipNSamplesY, int* autoPredict) {
	elem--;
	int code;
	if(argv[0] == string("-d") || argv[0] == string("'--decode"))
		*encode = 0;
	else if(argv[0] == string("-e") || argv[0] == string("'--encode"))
		*encode = 1;
	else if(argv[0] == string("-y") || argv[0] == string("'--lossy"))
		*lossy = 1;
	else if(argv[0] == string("-n") || argv[0] == string("'--lossless"))
		*lossy = 0;
	else {
		if(argv[0] == string("-i") || argv[0] == string("'--in")) {
		// fileIn
			code = handleFlagParse(elem, argv[1], string("-i"), string("--in"), string("filename"),
									string("input"), string("input_file"), string ("input file name"));
			if (code < 0)
				return -1;
			*fileIn = argv[1];
		} else if(argv[0] == string("-o") || argv[0] == string("'--out")) {
		// fileOut
			code = handleFlagParse(elem, argv[1], string("-o"), string("--out"), string("filename"),
									string("input"), string("output_file"), string ("output file name"));
			if (code < 0)
				return -1;
			*fileOut = argv[1];
		} else if(argv[0] == string("-w") || argv[0] == string("'--window")) {
		// windowSize
			code = handleFlagParse(elem, argv[1], string("-w"), string("--window"), string("size"),
									string("window size"), string("window_size"), string ("'m' calculation window"));
			if (code < 0)
				return -1;
			*windowSize = atoi(argv[1]);
		} else if(argv[0] == string("-s") || argv[0] == string("'--samples")) {
		// skipNSamples
			code = handleFlagParse(elem, argv[1], string("-s"), string("--samples"), string("number of samples"),
									string("amount of samples to be skipped"), string("number_of_samples"),
									string ("amount of samples to be skipped before recalculating 'm'"));
			if (code < 0)
				return -1;
			*skipNSamples = atoi(argv[1]);
		} else if(argv[0] == string("-q") || argv[0] == string("'--quantization")) {
		// quantBits
			code = handleFlagParse(elem, argv[1], string("-q"), string("--quantization"), string("quantization value"),
									string("bits to quantize"), string("number_of_bits"),
									string ("number of bits to be quantized in the predictor"));
			if (code < 0)
				return -1;
			*quantBits = atoi(argv[1]);
		} else if(argv[0] == string("-p") || argv[0] == string("'--predictor")) {
		// predictorOrder
			code = handleFlagParse(elem, argv[1], string("-p"), string("--predictor"), string("order"),
									string("predictor order"), string("order_of_predictor"),
									string ("order of the predictor\n		0 - no prediction\n		1 - linear model\n		2 - linear model\n 		3 - quadratic model"));

			if (code < 0)
				return -1;
			*predictorOrder = atoi(argv[1]);
		} else if(argv[0] == string("-b") || argv[0] == string("'--windowstereo")) {
		// windowSizeY
			code = handleFlagParse(elem, argv[1], string("-b"), string("--windowstereo"), string("size"),
									string("window size"), string("window_size"), string ("'m' calculation window for the channel difference"));
			if (code < 0)
				return -1;
			*windowSizeY = atoi(argv[1]);
		} else if(argv[0] == string("-c") || argv[0] == string("'--samplestero")) {
		// skipNSamples
			code = handleFlagParse(elem, argv[1], string("-c"), string("--samplestereo"), string("number of samples"),
									string("amount of samples to be skipped"), string("number_of_samples"),
									string ("amount of samples to be skipped before recalculating 'm' for the channel difference"));
			if (code < 0)
				return -1;
			*skipNSamplesY = atoi(argv[1]);
		} else if(argv[0] == string("-a") || argv[0] == string("'--auto")) {
		// autoPredict
			code = handleFlagParse(elem, argv[1], string("-a"), string("--auto"), string("motivation value"),
									string("automatic"), string("motivation_value"),
									string ("motivation of the automatic parametrizer"));

			if (code < 0)
				return -1;
			*autoPredict = atoi(argv[1]);
		} else {
			cout << "Error: invalid flag '" << argv[0] << endl;
			return -1;
		}
		argv++;
		elem--;
	}
	
	if (elem == 0) {
		bool valid = true;
		if ((*fileIn).empty()) {
			cout << "Error: missing input file name.\n"
				<< "	--in OR -i <input_file> : input file name" << endl;
			valid = false;
		}
		if ((*fileOut).empty()) {
			cout << "Error: missing output file name.\n"
				<< "	--out OR -o <output_file> : output file name" << endl;
			valid = false;
		}
		if (*encode == -1) {
			cout << "Error: missing operation type. One of the following flags is required:\n"
				 << "	--encode OR -e : encoding\n	--decode OR -d : decoding" << endl;
			valid = false;
		}
		// if encoding
		else if (*encode == 1){
			if(*lossy == -1) {
				cout << "Error: missing compression type. One of the following flags is required:\n"
					<< "	--lossy OR -y : lossy\n	--lossless OR -n : lossless" << endl;
				valid = false;
			// if lossy
			} else if (*lossy == 1) {
				if (*quantBits > 15 || *quantBits < -1) {
					cout << "Error: number of bits to quantize must be less than 16." << endl;
					valid = false;
				} else if (*quantBits == -1) {
					*quantBits = 6;
				}
			// if lossless
			} else if (*quantBits >= 0) {
				cout << "Error: number of quantized bits is only valid if the lossy mode is active." << endl;
				valid = false;
			}
			// no auto predict
			if (*autoPredict == -1) {
				*autoPredict = 0;
				if (*windowSize == -1) {
					*windowSize = 2000;
				}
				else if (*windowSize < 1) {
					cout << "Error: window size must be greater than zero." << endl;
					valid = false;
				} 
				if (*skipNSamples >= *windowSize || *skipNSamples < -1) {
					cout << "Error: amount of samples to skip must be less than the window size." << endl;
					valid = false;
				} else if (*skipNSamples == -1) {
					*skipNSamples = 100;
				}
				if (*windowSizeY == -1) {
					*windowSizeY = 2000;
				}
				else if (*windowSizeY < 1) {
					cout << "Error: window size (channel difference) must be greater than zero." << endl;
					valid = false;
				} 
				if (*skipNSamplesY >= *windowSizeY || *skipNSamplesY < -1) {
					cout << "Error: amount of samples to skip (channel difference) must be less than the window size." << endl;
					valid = false;
				} else if (*skipNSamplesY == -1) {
					*skipNSamplesY = 100;
				}
				if (*predictorOrder > 3) {
					cout << "Error: predictor order must be between 0 and 3." << endl
						<< "	0 - no prediction" << endl << "	1 - linear model" << endl
						<< "	2 - linear model " << endl << "	3 - quadratic model" << endl;
					valid = false;
				} else if (*predictorOrder < 0) {
					*predictorOrder = 3;
				}
			} else {
				if (*autoPredict < 0 || *autoPredict > 4) {
					cout << "Error: auto predict motivation value must be between 0 and 4." << endl
		 				<< "	0 - fastest" << endl << "	4 - most compressed" << endl;
					valid = false;
				}
				*windowSize = -1;
				*windowSizeY = -1;
				*skipNSamples = -1;
				*skipNSamplesY = -1;
			}
		// if decoding
		} else {
			if (*lossy >= 0) {
				cout << "Error: lossy and lossless compression is only available as an encoding argument." << endl;
				valid = false;
			}
			if (*windowSize >= 0) {
				cout << "Error: window size is only available as an encoding argument." << endl;
				valid = false;
			}
			if (*skipNSamples >= 0) {
				cout << "Error: amount of samples to skip is only available as an encoding argument." << endl;
				valid = false;
			}
			if (*windowSizeY >= 0) {
				cout << "Error: window size (channel difference) is only available as an encoding argument." << endl;
				valid = false;
			}
			if (*skipNSamplesY >= 0) {
				cout << "Error: amount of samples to skip (channel difference) is only available as an encoding argument." << endl;
				valid = false;
			}
			if (*predictorOrder >= 0) {
				cout << "Error: predictor order is only available as an encoding argument." << endl;
				valid = false;
			}
			if (*autoPredict >= 0) {
				cout << "Error: automatic prediction is only available as an encoding argument." << endl;
				valid = false;
			}
		}
		if (valid) {
			return 0;
		}
		return -1;
	}
	return parseArgs(elem, argv+1, encode, lossy, fileIn, fileOut, windowSize, skipNSamples, 
					 quantBits, predictorOrder, windowSizeY, skipNSamplesY, autoPredict);
}

void printUsage() {
	cout << "USAGE:\n	REQUIRED BASE FLAGS:" << endl
		 << "		--in OR -i <input_file> : input filename from where to read the data" << endl
		 << "		--out OR -o <output_file> : output filename where to write the data" << endl
		 << "		One of the following:" << endl
		 << "		--encode OR -e : encode the input file"	<< endl
		 << "		--decode OR -d : decode the input file\n" << endl
		 << "	REQUIRED ENCODING FLAGS:" << endl
		 << "		One of the following:" << endl
		 << "		--lossy OR -y : lossy encoding" << endl
		 << "		--lossless OR -n : lossless encoding\n" << endl
		 << "	OPTIONAL ENCODING FLAGS:" << endl
		 << "		--window OR -w : window size for which the m will be calculated in one channel" << endl
		 << "			RANGE:  > 1" << endl
		 << "		--samples OR -s : amount of samples to be skipped before calculating a new 'm' in one channel" << endl
		 << "			RANGE:  0 <= s < window" << endl
		 << "		--auto OR -a : automatically calculate an approximation to the 'ideal' values for the window and skip" << endl
		 << "			RANGE:  0 < a <= 4" << endl
		 << "				0 - fastest" << endl << "				4 - most compressed" << endl
		 << "		--predictor OR -p : order of the predictor" << endl
		 << "			RANGE:  0 <= p <= 3" << endl
		 << "				0 - no prediction" << endl << "				1 - linear model" << endl
		 << "				2 - linear model " << endl << "				3 - quadratic model\n" << endl
		 << "	OPTIONAL ENCODING FLAGS FOR STEREO ONLY:" << endl
		 << "		--windowstereo OR -b  : window size for the channel differences" << endl
		 << "			RANGE:  > 1" << endl
		 << "		--samplestereo OR -c : samples to be skipped for the channel differences" << endl
		 << "			RANGE:  0 <= c < windowstereo\n" << endl
		 << "	OPTIONAL LOSSY ENCODING FLAGS" << endl
		 << "		--quantization OR -q : number of bits to be quantized in the predictor" << endl
		 << "			RANGE:  0 <= q < 16" << endl;
}

int main(int argc,char** argv){
	if (argc < 6) {
		printUsage();
		return 1;
	}

	int encode = -1;
	int lossy = -1;
	string fileIn;
	string fileOut;
	int windowSize = -1;
	int skipNSamples = -1; 
	int quantBits = -1;
	int predictorOrder = -1;
	int windowSizeY = -1;
	int skipNSamplesY = -1;
	int autoPredict = -1;

	int valid = parseArgs(argc-1, argv+1, &encode, &lossy, &fileIn, &fileOut, &windowSize, &skipNSamples, 
						 &quantBits, &predictorOrder, &windowSizeY, &skipNSamplesY, &autoPredict);
	if (valid < 0) {
		cout << "EXITING: invalid parameters. Run without arguments to print usage." << endl;
		return -1;
	}

	// TODO get values from auto

	if (encode > 0) {
		cout << "Encoding '" << fileIn << "' to '" << fileOut << "'." << endl;
		if (lossy > 0)
			cout << "	- Applying lossy compression with a quantization of " << quantBits << " bits;" << endl;
		else 
			cout << "	- Applying lossless compression;" << endl;
		if (autoPredict > 0)
			cout << "	- Parameters related to 'm' were automatically calculated;" << endl;
		cout << "	- Window size: " << windowSize << ";" << endl;
		cout << "	- Skip " << skipNSamples << " samples before recalculating 'm';" << endl;
		cout << "	- Channel difference window size: " << windowSizeY << ";" << endl;
		cout << "	- Channel difference skip " << skipNSamplesY << " samples before recalculating 'm';" << endl;
		cout << "	- Predictor order: " << predictorOrder << ";" << endl;
	} else 
		cout << "Decoding '" << fileIn << "' to '" << fileOut << "'." << endl;

	//online_less(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]),atoi(argv[7]));
	//online_ly(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]),atoi(argv[7]),atoi(argv[8]));
}
