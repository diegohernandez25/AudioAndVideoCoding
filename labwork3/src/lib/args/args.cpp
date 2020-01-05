#include <iostream>
#include "args.h"

using namespace std;

args::args(int argc, char** argv, int mode) {
	if (mode < 0 || mode > 3)
		exit(0);

	args::mode = mode;
	
	if (argc < 5) {
		printUsage();
		exit(0);
	}

	jpegPredictor = -1;
	blockSize = -1;
	windowSize = -1;
	skipNPixels = -1; 
	macroSize = -1;
	searchArea = -1;
	searchDepth = -1;
	keyPeriodicity = -1;
	quantY = -1;
	quantU = -1;
	quantV = -1;
	dct = true;

	int valid = parseArgs(argc-1, argv+1);
	if (valid >= 0)
		valid = validateArgs();
	if (valid < 0) {
		cout << "EXITING: invalid parameters. Run without arguments to print usage." << endl;
		exit(0);
	}

	cout << (mode == 0 ? "Decoding '" : "Encoding '") << fileIn << "' to '" << fileOut << "'." << endl;
	if (mode > 0) {
		cout << "	- Predictor mode: ";
		if (jpegPredictor == 0) 
			cout << "off" << endl;
		else if (jpegPredictor < 8)
			cout << "JPEG linear predictor type " << jpegPredictor << endl;
		else if (jpegPredictor == 8)
			cout << "JPEG-LS" << endl;
		else
			cout << "Auto linear JPEG per block" << endl;
		if (blockSize > -1)
			cout << "	- Block size: " << blockSize << "x" << blockSize << " pixels" << endl;
		cout << "	- Window size: " << windowSize << endl
			 << "	- Skip " << skipNPixels << " pixels before recalculating 'm'" << endl;
	}
	if (mode > 1) {
		cout << "	- Macroblock size: " << macroSize << "x" << macroSize << " blocks" << endl
		 << "	- Search area: " << searchArea << " pixels" << endl
		 << "	- Search depth: " << searchDepth << " frames" << endl
		 << "	- Periodicity of key frames: " << keyPeriodicity << endl;
	}
	if (mode == 3) {
		if (dct)
			cout << "	- Using DCT" << endl;
		else
			cout << "	- Quantization of Y, U, V: " << quantY << ", " << quantU << ", " << quantV << endl;
	}
}

void args::printUsage() {
	cout << "USAGE:\n	REQUIRED FLAGS:" << endl
		 << "		--in OR -i <input_file> : input filename from where to read the data" << endl
		 << "		--out OR -o <output_file> : output filename where to write the data" << endl;
	if (mode > 0) {
		cout << "	OPTIONAL FLAGS:" << endl
		 << "		--predictor OR -p : predictor mode" << endl
		 << "			RANGE:  0 <= p <= 9" << endl
		 << "				0 - no prediction" << endl
		 << "				1-7 - JPEG linear predictors" << endl
		 << "				8 - non-linear predictor JPEG-LS" << endl
		 << "				9 - auto-find best JPEG linear predictor for each block " << endl
		 << "		--blocksize OR -b : block size for inter-frame coding" << endl
		 << "			RANGE: > 0" << endl
		 << "		--window OR -w : window size for which the m will be calculated" << endl
		 << "			RANGE:  > 1" << endl
		 << "		--pixels OR -x : amount of pixels to be skipped before calculating a new 'm'" << endl
		 << "			RANGE:  0 <= x < window" << endl;
	}
	if (mode > 1) {
		cout << "		--macrosize OR -m : macro size for inter-frame coding" << endl
		 << "			RANGE: > 0" << endl
		 << "		--searcharea OR -a : search area for inter-frame coding" << endl
		 << "			RANGE: > 0" << endl
		 << "		--searchdepth OR -d : search depth for inter-frame coding" << endl
		 << "			RANGE: > 0" << endl
		 << "		--keyperiodicity OR -k : periodicity of the key frames. 0 means only the first frame is guaranteed to be fully I." << endl
		 << "			RANGE: >= 0" << endl;
	}
	if (mode == 3) {
		cout <<  "		--quant OR -q : simple quantization instead of using DCT" << endl
		 <<"		--quantY OR -y : quantization steps for the prediction residuals of Y" << endl
		 << "		--quantU OR -u : quantization steps for the prediction residuals of U" << endl
		 << "		--quantV OR -v : quantization steps for the prediction residuals of V" << endl
		 << "			RANGE:  0 <= q <= 8" << endl;
	}
}

int args::parseArgs(int elem, char** argv) {
	elem--;
	int code;
	if(argv[0] == string("-q") || argv[0] == string("'--quant")) {
	// dct
		dct = false;
	} else {
		if(argv[0] == string("-i") || argv[0] == string("'--in")) {
		// fileIn
			code = handleFlagParse(elem, argv[1], string("-i"), string("--in"), string("filename"),
									string("input"), string("input_file"), string ("input file name"));
			if (code < 0)
				return -1;
			fileIn = argv[1];
		} else if(argv[0] == string("-o") || argv[0] == string("'--out")) {
		// fileOut
			code = handleFlagParse(elem, argv[1], string("-o"), string("--out"), string("filename"),
									string("input"), string("output_file"), string ("output file name"));
			if (code < 0)
				return -1;
			fileOut = argv[1];
		} else if(argv[0] == string("-p") || argv[0] == string("'--predictor")) {
		// jpegPredictor
			code = handleFlagParse(elem, argv[1], string("-p"), string("--predictor"), string("predictor mode"),
									string("JPEG predictor"), string("predictor_mode"),
									string ("mode of the JPEG predictor"));
			if (code < 0)
				return -1;
			jpegPredictor = atoi(argv[1]);
		} else if(argv[0] == string("-b") || argv[0] == string("'--blocksize")) {
		// blockSize
			code = handleFlagParse(elem, argv[1], string("-b"), string("--blocksize"), string("block size"),
									string("width of block in pixels"), string("block_size"),
									string ("block size"));
			if (code < 0)
				return -1;
			blockSize = atoi(argv[1]);
		} else if(argv[0] == string("-w") || argv[0] == string("'--window")) {
		// windowSize
			code = handleFlagParse(elem, argv[1], string("-w"), string("--window"), string("size"),
									string("window size"), string("window_size"), string ("'m' calculation window"));
			if (code < 0)
				return -1;
			windowSize = atoi(argv[1]);
		} else if(argv[0] == string("-x") || argv[0] == string("'--pixels")) {
		// skipNPixels
			code = handleFlagParse(elem, argv[1], string("-x"), string("--pixels"), string("number of pixels"),
									string("amount of pixels to be skipped"), string("number_of_pixels"),
									string ("amount of pixels to be skipped before recalculating 'm'"));
			if (code < 0)
				return -1;
			skipNPixels = atoi(argv[1]);
		} else if(argv[0] == string("-m") || argv[0] == string("'--macrosize")) {
		// macroSize
			code = handleFlagParse(elem, argv[1], string("-m"), string("--macrosize"), string("macroblock size"),
									string("width of macroblock in block for inter-frame prediction"), string("macroblock_size"),
									string ("macroblock size"));
			if (code < 0)
				return -1;
			macroSize = atoi(argv[1]);
		} else if(argv[0] == string("-a") || argv[0] == string("'--searcharea")) {
		// searchArea
			code = handleFlagParse(elem, argv[1], string("-a"), string("--searcharea"), string("search area"),
									string("search area for inter-frame prediction"), string("search_area"),
									string ("search area size"));
			if (code < 0)
				return -1;
			searchArea = atoi(argv[1]);
		} else if(argv[0] == string("-d") || argv[0] == string("'--searchdepth")) {
		// searchDepth
			code = handleFlagParse(elem, argv[1], string("-d"), string("--searchdepth"), string("search depth"),
									string("search depth for inter-frame prediction"), string("search_depth"),
									string ("search depth length"));
			if (code < 0)
				return -1;
			searchDepth = atoi(argv[1]);
		} else if(argv[0] == string("-k") || argv[0] == string("'--keyperiodicity")) {
		// keyPeriodicity
			code = handleFlagParse(elem, argv[1], string("-k"), string("--keyperiodicity"), string("key frame periodicity"),
									string(""), string("key_periodicity"),
									string ("periodicity of key frames for inter-frame prediction"));
			if (code < 0)
				return -1;
			keyPeriodicity = atoi(argv[1]);
		} else if(argv[0] == string("-y") || argv[0] == string("'--quantY")) {
		// quantY
			code = handleFlagParse(elem, argv[1], string("-y"), string("--quantY"), string("quantization value for Y"),
									string("bits to quantize"), string("number_of_bits"),
									string ("number of bits to be quantized in Y residuals"));
			if (code < 0)
				return -1;
			quantY = atoi(argv[1]);
			dct = false;
		} else if(argv[0] == string("-u") || argv[0] == string("'--quantU")) {
		// quantU
			code = handleFlagParse(elem, argv[1], string("-u"), string("--quantU"), string("quantization value for U"),
									string("bits to quantize"), string("number_of_bits"),
									string ("number of bits to be quantized in U residuals"));
			if (code < 0)
				return -1;
			quantU = atoi(argv[1]);
			dct = false;
		} else if(argv[0] == string("-v") || argv[0] == string("'--quantV")) {
		// quantV
			code = handleFlagParse(elem, argv[1], string("-v"), string("--quantV"), string("quantization value for V"),
									string("bits to quantize"), string("number_of_bits"),
									string ("number of bits to be quantized in V residuals"));
			if (code < 0)
				return -1;
			quantV = atoi(argv[1]);
			dct = false;
		} else {
			cout << "Error: invalid flag '" << argv[0] << "'" << endl;
			return -1;
		}
		argv++;
		elem--;
	}
	
	if (elem <= 0)
		return 0;
	return parseArgs(elem, argv+1);
}

int args::validateArgs() {
	bool valid = true;
	// Files
	if ((fileIn).empty()) {
		cout << "Error: missing input file name.\n"
			<< "	--in OR -i <input_file> : input file name" << endl;
		valid = false;
	}
	if ((fileOut).empty()) {
		cout << "Error: missing output file name.\n"
			<< "	--out OR -o <output_file> : output file name" << endl;
		valid = false;
	}

	if (mode < 1) {
		if (jpegPredictor >= 0 || blockSize >= 0) {
			cout << "Error: predictor mode is only available as an encoding argument." << endl;
			valid = false;
		}
		if (windowSize >= 0) {
			cout << "Error: window size is only available as an encoding argument." << endl;
			valid = false;
		}
		if (skipNPixels >= 0) {
			cout << "Error: window size is only available as an encoding argument." << endl;
			valid = false;
		}
	} else {
		// Predictor
		if (jpegPredictor > 9) {
			cout << "Error: predictor order must be between 0 and 9." << endl
				<< "				0 - no prediction" << endl
				<< "				1-7 - JPEG linear predictors" << endl
				<< "				8 - non-linear predictor JPEG-LS" << endl
		 		<< "				9 - auto-find best JPEG linear predictor for each block " << endl;
			valid = false;
		} else if (jpegPredictor < 0) {
			jpegPredictor = 9;
		}

		// Block size
		if (mode < 2) {
			if (jpegPredictor < 9 && blockSize > -1) {
				cout << "Error: block size can only be set in auto JPEG mode." << endl;
				valid = false;
			} else if (jpegPredictor == 9 && blockSize == -1)
				blockSize = 8;
		} else {
			if (blockSize == -1)
				blockSize = 8;
			if (blockSize < 1) {
				cout << "Error: block size must be greater than zero." << endl;
				valid = false;
			}
		}
		if (blockSize%2 != 0) {
			cout << "Error: block size must be an even number." << endl;
			valid = false;
		}

		// TODO replace values, see if necessary
		// Window
		if (windowSize == -1) {
			windowSize = 128;
		}
		else if (windowSize < 1) {
			cout << "Error: window size must be greater than zero." << endl;
			valid = false;
		}

		// Skip pixels
		if (skipNPixels >= windowSize || skipNPixels < -1) {
			cout << "Error: amount of pixels to skip must be less than the window size." << endl;
			valid = false;
		} else if (skipNPixels == -1) {
			skipNPixels = 32;
		}
	}

	if (mode < 2) {
		if (macroSize + searchArea + keyPeriodicity != -3) {
			cout << "Error: macroblock size, search area and key frame periodicity are only available on inter-frame encoder." << endl;
			valid = false;
		}
	} else {
		// Macro block size
		if (macroSize == -1)
			macroSize = 2;
		if (macroSize < 1) {
			cout << "Error: macroblock size must be greater than zero." << endl;
			valid = false;
		}

		// Search area
		if (searchArea == -1)
			searchArea = 4;
		if (searchArea < 1) {
			cout << "Error: search area must be greater than zero." << endl;
			valid = false;
		}

		// Search depth
		if (searchDepth == -1)
			searchDepth = 4;
		if (searchDepth < 1) {
			cout << "Error: search depth must be greater than zero." << endl;
			valid = false;
		}

		// Key periodicity
		if (keyPeriodicity == -1)
			keyPeriodicity = 0;
		if (keyPeriodicity < 0) {
			cout << "Error: key frame periodicity must be at least zero." << endl;
			valid = false;
		}
	}

	if (mode < 3) {
		if (quantY + quantU + quantV != -3 || !dct) {
			cout << "Error: quantization and DCT are only available on lossy encoder." << endl;
			valid = false;
		}
	} else {
		// Quantization
		if (!dct) {
			if (quantY > 8 || quantY < -1) {
				cout << "Error: number of bits to quantize must be less or equal to 8." << endl;
				valid = false;
			} else if (quantY == -1) {
				quantY = 2;
			}
			if (quantU > 8 || quantU < -1) {
				cout << "Error: number of bits to quantize must be less or equal to 8." << endl;
				valid = false;
			} else if (quantU == -1) {
				quantU = 2;
			}
			if (quantV > 8 || quantV < -1) {
				cout << "Error: number of bits to quantize must be less or equal to 8." << endl;
				valid = false;
			} else if (quantV == -1) {
				quantV = 2;
			}
		}
	}
	
	if (valid) {
		return 0;
	}
	return -1;
}

int args::handleFlagParse(int elem, char* next, string shortFlag, string longFlag, string value,
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
