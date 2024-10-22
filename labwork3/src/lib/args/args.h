#ifndef ARGS_H
#define ARGS_H

#include <iostream>

using namespace std;

class args {
	public:
        args(int argc, char** argv, double mode);

		string fileIn;
        string fileOut;
        int jpegPredictor;
        int blockSize;
        int macroSize;
        int searchArea;
        int searchDepth;
        int keyPeriodicity;
        int qualY;
        int qualU;
        int qualV;
        bool dct;

	private:
        int mode;
		void printUsage();
        int validateArgs();
        int parseArgs(int elem, char** argv);
        int handleFlagParse(int elem, char* next, string shortFlag, string longFlag, string value,
                            string flagMeaning, string placeholder, string description);
};

#endif