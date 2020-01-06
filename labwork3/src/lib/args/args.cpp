#include <iostream>
#include "args.h"

using namespace std;

args::args(int argc, char** argv, double mode) {
	if (mode < 0 || mode > 4)
		exit(0);

	args::mode = mode;
	
	if (mode == 4)
		mode = 0.5;

	if (argc < 5) {
		printUsage();
		exit(0);
	}

	jpegPredictor = -1;
	blockSize = -1;
	macroSize = -1;
	searchArea = -1;
	searchDepth = -1;
	keyPeriodicity = -1;
	qualY = -1;
	qualU = -1;
	qualV = -1;
	dct = false;

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
	}
	if (mode > 1) {
		cout << "	- Macroblock size: " << macroSize << "x" << macroSize << " blocks" << endl
		 << "	- Search area: " << searchArea << " pixels" << endl
		 << "	- Search depth: " << searchDepth << " frames" << endl
		 << "	- Periodicity of key frames: " << keyPeriodicity << endl;
	}
	if (mode == 3) {
		if (dct)
			cout << "	- Using DCT with levels of Y, U, V: ";
		else
			cout << "	- Quantization levels of Y, U, V: ";
		cout << qualY << ", " << qualU << ", " << qualV << endl;
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
		 << "		--blocksize OR -b : block size in pixels" << endl
		 << "			RANGE: > 0" << endl;
	}
	if (mode > 1) {
		cout << "		--macrosize OR -m : macro size for inter-frame coding" << endl
		 << "			RANGE: > 0" << endl
		 << "		--searcharea OR -a : search area for inter-frame coding" << endl
		 << "			RANGE: > 0" << endl
		 << "		--searchdepth OR -d : search depth for inter-frame coding" << endl
		 << "			RANGE:  0 < d < 16" << endl
		 << "		--keyperiodicity OR -k : periodicity of the key frames. 0 means only the first frame is guaranteed to be fully I." << endl
		 << "			RANGE: >= 0" << endl;
	}
	if (mode == 3) {
		cout <<  "		--dct OR -q : use DCT instead of using simple quantization" << endl
		 <<"		--qualY OR -y : quality level for the prediction residuals of Y" << endl
		 << "		--qualU OR -u : quality level for the prediction residuals of U" << endl
		 << "		--qualV OR -v : quality level for the prediction residuals of V" << endl
		 << "			RANGE:  >= 0 (lower means better)" << endl;
	}
}

int args::parseArgs(int elem, char** argv) {
	elem--;
	int code;
	if(argv[0] == string("-q") || argv[0] == string("'--quant")) {
	// dct
		dct = true;
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
		} else if(argv[0] == string("-y") || argv[0] == string("'--qualY")) {
		// qualY
			code = handleFlagParse(elem, argv[1], string("-y"), string("--qualY"), string("quality level for Y"),
									string("level of quality"), string("level"),
									string ("quality level for quantization in Y residuals"));
			if (code < 0)
				return -1;
			qualY = atoi(argv[1]);
		} else if(argv[0] == string("-u") || argv[0] == string("'--qualU")) {
		// qualU
			code = handleFlagParse(elem, argv[1], string("-u"), string("--qualU"), string("quality level for U"),
									string("level of quality"), string("level"),
									string ("quality level for quantization in U residuals"));
			if (code < 0)
				return -1;
			qualU = atoi(argv[1]);
		} else if(argv[0] == string("-v") || argv[0] == string("'--qualV")) {
		// qualV
			code = handleFlagParse(elem, argv[1], string("-v"), string("--qualV"), string("quality level for V"),
									string("level of quality"), string("level"),
									string ("quality level for quantization in V residuals"));
			if (code < 0)
				return -1;
			qualV = atoi(argv[1]);
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

	if (mode == 0) {
		if (jpegPredictor >= 0 || blockSize >= 0) {
			cout << "Error: predictor mode is only available as an encoding argument." << endl;
			valid = false;
		}
	} else {
		if (mode > 0.5) {
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
			if (blockSize%2 != 0) {
				cout << "Error: block size must be an even number." << endl;
				valid = false;
			}
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
		if (searchDepth < 1 || searchDepth > 15) {
			cout << "Error: search depth must be greater than zero and less than 16." << endl;
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
		if (qualY + qualU + qualV != -3 || dct) {
			cout << "Error: quantization and DCT are only available on lossy encoder." << endl;
			valid = false;
		}
	} else {
		// Quantization
		// TODO change defaults for dct?
		if (qualY < -1) {
			cout << "Error: level of quality must be less or equal to 0." << endl;
			valid = false;
		} else if (qualY == -1) {
			qualY = 10;
		}
		if (qualU < -1) {
			cout << "Error: level of quality must be less or equal to 0." << endl;
			valid = false;
		} else if (qualU == -1) {
			qualU = 10;
		}
		if (qualV < -1) {
			cout << "Error: level of quality must be less or equal to 0." << endl;
			valid = false;
		} else if (qualV == -1) {
			qualV = 10;
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
