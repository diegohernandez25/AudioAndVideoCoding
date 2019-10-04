#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include "Wav/Wav.h"

using namespace std;

int main() {
    string infile;
    cout << "input file (must be .wav file):";

    getline(cin, infile);
    cout << "input path:\t" << infile << endl;

    Wav mywav   = Wav(infile);
    cout << "file size:\t" << to_string(mywav.getFileSize()) << endl;
    mywav.readHeader();

    string outfile;
    cout << "output file path/name (must be .wav file):";
    getline(cin, outfile);
    cout << "output path:\t" << outfile << endl;

    const char* fileOutput  = outfile.c_str();
    FILE* outFile    = fopen(fileOutput,"w");
    mywav.cpBySample(outFile);

    return 0;
}