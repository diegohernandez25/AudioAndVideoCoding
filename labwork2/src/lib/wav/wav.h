//
// Created by diego on 17/11/19.
//

#ifndef WAV_WAV_H
#define WAV_WAV_H


#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

using namespace std;



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



class wav {
public:

    wav_hdr wavHeader;

    int bytesPerSample;
    int numSamples;


    vector<signed short> data;

    wav(string filename)
    {   signed char* ptrData;
        int headerSize = sizeof(wav_hdr);
        ptrData = (signed char*) malloc(wavHeader.Subchunk2Size);

        FILE* inwavfile   = fopen(filename.c_str(),"r");
        fread(&wavHeader, 1, (size_t) headerSize, inwavfile);
        fread(ptrData, 1, wavHeader.Subchunk2Size, inwavfile);
        fclose(inwavfile);

        bytesPerSample = wavHeader.bitsPerSample/8;
        numSamples = wavHeader.Subchunk2Size/bytesPerSample;
        string tmp;
        cout << "num samples: "<<numSamples << endl;
        cout << "num sample per channel:" << numSamples/wavHeader.NumOfChan << endl;

        for(signed int i = 0; i< wavHeader.Subchunk2Size; i+=bytesPerSample)
        {   signed char *pchar  = ptrData + i;
            short samp =  (((short)pchar[1])<<8) | (0x00ff & pchar[0]);
            data.push_back(samp);
        }
        cout << "vec size:" << data.size() << endl;
        free(ptrData);
    }

    vector<short> getChannelData(int nch);
    void printHeader();
private:

};


#endif //WAV_WAV_H
