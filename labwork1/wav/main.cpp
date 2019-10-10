#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include "Wav/Wav.h"

void test()
{
    Wav mywav   = Wav("/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample02.wav");
    mywav.readHeader();

    string path = "/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample02_cpy.wav";
    FILE* outFile    = fopen(path.c_str(),"w");
    mywav.cpBySample(outFile);
    fclose(outFile);
    cout << "Done copy"<< endl;

    path = "/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample02_channel1.wav";
    outFile    = fopen(path.c_str(),"w");
    mywav.recordChannel(1,outFile);
    fclose(outFile);
    cout << "Done channel 1"<< endl;

    path = "/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample02_channel2.wav";
    outFile    = fopen(path.c_str(),"w");
    mywav.recordChannel(2,outFile);
    fclose(outFile);
    cout << "Done channel 2"<< endl;

    mywav.freeChannels();
    cout << "Channels freed" << endl;

    path = "/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample02_midrise.wav";
    outFile    = fopen(path.c_str(),"w");
    mywav.encMidriseUniQuant(12, outFile);
    fclose(outFile);
    cout << "Done Midrise Quantization"<< endl;

    path = "/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/wav/sample02_midtread.wav";
    outFile    = fopen(path.c_str(),"w");
    mywav.encMidtreadUniQuant(12, outFile);
    fclose(outFile);
    cout << "Done Midtread Quantization"<< endl;
}



int main() {
    test();
    return 0;
}