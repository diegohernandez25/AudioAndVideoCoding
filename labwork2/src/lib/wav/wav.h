//
// Created by diego on 17/11/19.
//

#ifndef WAV_WAV_H
#define WAV_WAV_H


#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <iterator>

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

    //<Constructors
    wav(string filename);

    //<Methods
    vector<short> getChannelData(int nch);
    void printHeader();
    void writeFile(FILE* outfile, wav_hdr header, vector<short> data);


    //<  Setters
    void setSubchunk2Size(uint32_t val);
    void setBytesPerSec(uint32_t val);
    void setSamplesPerSec(uint32_t val);
    void setChunkSize(uint32_t val);
    void setNumOfChannels(uint16_t val);
    void setBitsPerSample(uint16_t val);
    void setAudioFormat(uint16_t val);
    void setBytesPerSample(uint16_t val);
    void setNumSamples(uint32_t val);
    void setTotalSamples(uint32_t val);

    void setBlockAlign(uint16_t val);
    void setWavHeader(wav_hdr header);
    void setWavData(vector<short> data);
    void setFile(string name);

    //< Getters
    uint32_t getSubchunk2Size();
    uint32_t getBytesPerSec();
    uint32_t getSamplesPerSec();
    uint32_t getChunkSize();
    uint16_t getNumOfChannels();
    uint16_t getBitsPerSample();
    uint16_t getAudioFormat();
    uint16_t getBlockAlign();
    uint16_t getBytesPerSample();
    uint32_t getNumSamples();
    uint32_t getTotalSamples();


    wav_hdr getWavHeader();
    vector<short> getWavData();
    string getFile();

    //< Load file
    void load();

    //<Reserve Data
    void reserve(uint16_t n_samples, uint32_t n_ch);

    //Insert data
    void insert(uint32_t n_sample, uint16_t ch, uint32_t value);

    //Write a file
    void dump(FILE* outfile);

private:

    //<Attributes

    //<File name
    string fileName;

    //< Wav Header
    wav_hdr wavHeader;

    //< Bytes per sample
    uint16_t bytesPerSample;

    //< Number of Samples Per Channel
    uint32_t numSamples;

    //< Total number of samples.
    uint32_t totalSamples;

    //< Wav Data
    vector<short> wavData;
};


#endif //WAV_WAV_H
