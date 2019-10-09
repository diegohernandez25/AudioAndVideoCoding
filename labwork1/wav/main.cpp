#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include "Wav/Wav.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/plot.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/mat.hpp"



using namespace std;
using namespace cv;

void ex1()
{   string infile;
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


}

void test_plot()
{
    Wav mywav   = Wav("/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample05.wav");
    //mywav.readHeader();

    string path = "/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample05_quant.wav";
    FILE* outFile    = fopen(path.c_str(),"w");
    mywav.encMidriseUniQuant(15, outFile);
    fclose(outFile);

}

void debug()
{
    signed int b =  0xffff8000;
    cout << b << endl;
    cout << dec << -32768<< " " << hex <<  -32768;

}

int main() {
    test_plot();
    cout<<"no prob"<<endl;
    return 0;
}