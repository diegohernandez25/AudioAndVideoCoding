////<
///< Created by diego on 03-10-2019.
///<

#ifndef TEST_WAV_H
#define TEST_WAV_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <string>
#include <cmath>
#include <fstream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/plot.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/mat.hpp"
#include "gnuplot.h"

using namespace std;
using namespace cv;
/**
 * Wav header structure.
 * */
typedef struct  WAV_HEADER
{
    /* RIFF Chunk Descriptor */
    uint8_t         RIFF[4];        ///< RIFF Header Magic header
    uint32_t        ChunkSize;      ///< RIFF Chunk Size
    uint8_t         WAVE[4];        ///< WAVE Header
    uint8_t         fmt[4];         ///< FMT header
    uint32_t        Subchunk1Size;  ///< Size of the fmt chunk
    uint16_t        AudioFormat;    ///< Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        NumOfChan;      ///< Number of channels 1=Mono 2=Sterio
    uint32_t        SamplesPerSec;  ///< Sampling Frequency in Hz
    uint32_t        bytesPerSec;    ///< bytes per second
    uint16_t        blockAlign;     ///< 2=16-bit mono, 4=16-bit stereo
    uint16_t        bitsPerSample;  ///< Number of bits per sample
    uint8_t         Subchunk2ID[4]; ///< "data"  string
    uint32_t        Subchunk2Size;  ///< Sampled data length
} wav_hdr;

class Wav {
public:

    #define MIDRISE_QUANT 1
    #define MIDTREAD_QUANT 2


    //Attributes
    string filePath;
    wav_hdr wavHeader;

    int bytesPerSample;
    int numSamples;

    signed char* wavData;
    vector<signed char *> data_channels;

    //Constructors
    Wav(string fileName)
    {   filePath    = fileName;
        int headerSize = sizeof(wav_hdr);
        FILE* inwavfile   = fopen(filePath.c_str(),"r");

        fread(&wavHeader, 1, (size_t) headerSize, inwavfile);

        wavData = (signed char*) malloc(wavHeader.Subchunk2Size);
        fread(wavData, 1, wavHeader.Subchunk2Size, inwavfile);

        bytesPerSample = wavHeader.bitsPerSample/8;
        numSamples = wavHeader.Subchunk2Size/bytesPerSample;
        fclose(inwavfile);
    }

    //Methods
    int getFileSize();
    void cpBySample(FILE *outWavfile);
    void readHeader();
    void splitChannels();
    void freeChannels();
    int recordChannel(uint16_t chn, FILE *outFilePath);
    int encMidriseUniQuant(int nbits, FILE *outfile);
    int encMidtreadUniQuant(int nbits, FILE *outfile);

    tuple<double, double> getSNR(char typeQuant, int nbits, int chn);
    int midriseQuant(int nbits, signed char* p, int size);
    int midtreadQuant(int nbits, signed char* p, int size);

    void plotWav();
    void plotChannels();
    void plotSamples(const vector<double>& samples, const string& title);
};

#endif //TEST_WAV_H
