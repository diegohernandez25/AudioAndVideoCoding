#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include "Wav/Wav.h"

void test()
{   cout << "Test:" <<endl;
    cout << "-----" <<endl;
    string path;
    cout << "Name of input WAV file (file must end in .wav):";
    getline(cin,path);
    cout << endl;

    Wav mywav   = Wav(path);

    cout << "Wav Header:" << endl;
    cout << "-----------" << endl;
    mywav.readHeader();
    cout << endl;

    cout << "Copy Sample by Sample WAV file:" << endl;
    cout << "-------------------------------" << endl;
    cout << "Name of file to copy to:";
    getline(cin,path);
    FILE* outFile    = fopen(path.c_str(),"w");
    mywav.cpBySample(outFile);
    fclose(outFile);
    cout << "Copy done."<< endl;
    cout << endl;

    cout << "Audio Channel 1 Extraction:" << endl;
    cout << "-------------------------------" << endl;
    cout << "Name of file to copy to:";
    getline(cin,path);
    outFile    = fopen(path.c_str(),"w");
    mywav.recordChannel(1,outFile);
    fclose(outFile);
    cout << "Done channel 1 extraction"<<endl;
    cout << endl;

    cout << "Audio Channel 2 Extraction:" << endl;
    cout << "-------------------------------" << endl;
    cout << "Name of file to copy to:";
    getline(cin,path);
    outFile    = fopen(path.c_str(),"w");
    mywav.recordChannel(2,outFile);
    fclose(outFile);
    cout << "Done channel 1 extraction"<<endl;
    cout << endl;

    mywav.freeChannels();

    cout << "Midrise Quantization:" << endl;
    cout << "-------------------------------" << endl;
    cout << "Number of bits to truncate to 0(cannot surpass max number of bits per sample):";
    getline(cin,path);
    int nbits_midrise = stoi(path);
    cout << "Name of file to copy to:";
    getline(cin,path);
    outFile    = fopen(path.c_str(),"w");
    mywav.encMidriseUniQuant(nbits_midrise, outFile);
    fclose(outFile);
    cout << "Done Midrise Quantization"<< endl;
    cout << endl;

    cout << "Midtread Quantization:" << endl;
    cout << "-------------------------------" << endl;
    cout << "Number of bits to truncate to 0(cannot surpass max number of bits per sample):";
    getline(cin,path);
    int nbits_midtread = stoi(path);
    cout << "Name of file to copy to:";
    getline(cin,path);
    outFile    = fopen(path.c_str(),"w");
    mywav.encMidtreadUniQuant(nbits_midtread, outFile);
    fclose(outFile);
    cout << "Done Midtread Quantization"<< endl;
    cout << endl;

    cout << "SNR Midrise Quant Channels."<<endl;
    cout << "---------------------------"<<endl;
    tuple<double, double> res = mywav.getSNR(MIDRISE_QUANT,nbits_midrise,1);
    cout << "[Channel 1] SNR Midrise Quant:\t" << to_string(get<0>(res)) << endl;
    cout << "[Channel 1] Maximum Absolute Error:\t"<< to_string((int) get<1>(res)) << endl;

    tuple<double, double> res2 = mywav.getSNR(MIDRISE_QUANT,nbits_midrise,2);
    cout << "[Channel 2] SNR Midrise Quant:\t" << to_string(get<0>(res2)) << endl;
    cout << "[Channel 2] Maximum Absolute Error:\t"<< to_string((int) get<1>(res2)) << endl;
    cout << endl;

    cout << "SNR Midtread Quant Channels."<<endl;
    cout << "---------------------------"<<endl;
    res = mywav.getSNR(MIDTREAD_QUANT,nbits_midtread,1);
    cout << "[Channel 1] SNR Midtread Quant:\t" << to_string( get<0>(res)) << endl;
    cout << "[Channel 1] Maximum Absolute Error:\t"<< to_string((int) get<1>(res)) << endl;

    res = mywav.getSNR(MIDTREAD_QUANT,nbits_midtread,2);
    cout << "[Channel 2] SNR Midtread Quant:\t" << to_string( get<0>(res)) << endl;
    cout << "[Channel 2] Maximum Absolute Error:\t"<< to_string((int) get<1>(res)) << endl;
    cout << endl;

    cout <<endl;
}


int main() {

    test();
    return 0;
}