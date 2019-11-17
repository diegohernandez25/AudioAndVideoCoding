//
// Created by diego on 17/11/19.
//
#include "wav.h"


wav::wav(string filename)
{   fileName = filename;

    string riff = "RIFF";
    const auto *p = reinterpret_cast<const uint8_t*>(riff.c_str());
    for(int i = 0; i< 4; i++)
        wavHeader.RIFF[i] = *(p + i);

    string wave = "WAVE";
    const auto *p1 = reinterpret_cast<const uint8_t*>(wave.c_str());
    for(int i = 0; i< 4; i++)
        wavHeader.WAVE[i] = *(p1 + i);

    string fmt = "fmt ";
    const auto *p2 = reinterpret_cast<const uint8_t*>(fmt.c_str());
    for(int i = 0; i< 4; i++)
        wavHeader.fmt[i] = *(p2 + i);

    string data = "data";
    const auto *p3 = reinterpret_cast<const uint8_t*>(data.c_str());
    for(int i = 0; i< 4; i++)
        wavHeader.Subchunk2ID[i] = *(p3 + i);
}

vector<short> wav::getChannelData(int nch)
{   vector<short> tmp;
    if(nch < 1 || nch > wav::wavHeader.NumOfChan) {
        cout << "Invalid channel selection." << endl;
        return tmp;
    }
    auto it = wav::wavData.begin() + (nch - 1);
    for(int j=0; it!=wavData.end() - wav::wavHeader.NumOfChan + (nch +1); it+= wav::wavHeader.NumOfChan,j++) {
        tmp.push_back(*it);
    }
    return tmp;
}

void wav::writeFile(FILE* outfile, wav_hdr header, vector<short> data)
{   fwrite(&header, 1, (size_t) sizeof(wav_hdr), outfile);
    char tmp[2] = {0};
    for(short & it : data)
    {   tmp[0] = it; tmp[1] = it >> 8;
        fwrite(tmp, sizeof(char), 2, outfile);
    }
}

void wav::dump(FILE* outfile){
    fwrite(&wavHeader, 1, (size_t) sizeof(wav_hdr), outfile);
    char tmp[2] = {0};
    for(short & it : wavData)
    {   tmp[0] = it; tmp[1] = it >> 8;
        fwrite(tmp, sizeof(char), 2, outfile);
    }
}

void wav::load(){
    char* ptrData;
    int headerSize = sizeof(wav_hdr);

    //< Loads Header
    FILE* infile   = fopen(fileName.c_str(),"r");
    fread(&wavHeader, 1, (size_t) headerSize, infile);

    //< Loads Data
    ptrData = (char*) malloc(wavHeader.Subchunk2Size);
    fread(ptrData, 1, wavHeader.Subchunk2Size, infile);
    fclose(infile);


    bytesPerSample = wavHeader.bitsPerSample/8;
    totalSamples = wavHeader.Subchunk2Size/bytesPerSample;
    numSamples = totalSamples / wavHeader.NumOfChan;

    vector<short> tmpData(totalSamples);

    for(signed int i = 0, j = 0; i< wavHeader.Subchunk2Size; i+=bytesPerSample, j++)
    {   char *pchar  = ptrData + i;
        short samp =  (((short)pchar[1])<<8) | (0x00ff & pchar[0]);
        tmpData[j] = samp;
    }

    wav::wavData = tmpData;
    free(ptrData);
}

void wav::reserve(uint16_t n_samples, uint32_t n_ch)
{   numSamples = n_samples;
    wavHeader.NumOfChan = n_ch;
    totalSamples = n_samples * n_ch;

    vector<short> tmp(totalSamples);
    wavData = tmp;
}

void wav::insert(uint32_t n_sample, uint16_t ch, uint32_t value){
    wavData[(n_sample) * wavHeader.NumOfChan + ch - 1] = value;
}

short wav::get(uint32_t n_sample, uint16_t ch){
    return wavData[(n_sample) * wavHeader.NumOfChan + ch - 1];
}

/**
 * Parses Wav file header information.
 * Reference: http:///<www.topherlee.com/software/pcm-tut-wavformat.html
 * */
void wav::printHeader()
{   cout << "RIFF header                :" << wav::wavHeader.RIFF[0] << wav::wavHeader.RIFF[1] << wav::wavHeader.RIFF[2] << wav::wavHeader.RIFF[3] << endl;
    cout << "WAVE header                :" << wav::wavHeader.WAVE[0] << wav::wavHeader.WAVE[1] << wav::wavHeader.WAVE[2] << wav::wavHeader.WAVE[3] << endl;
    cout << "FMT                        :" << wav::wavHeader.fmt[0] << wav::wavHeader.fmt[1] << wav::wavHeader.fmt[2] << wav::wavHeader.fmt[3] << endl;
    cout << "Data size                  :" << wav::wavHeader.ChunkSize << endl;

    ///< Display the sampling Rate from the header
    cout << "Sampling Rate              :" << wav::wavHeader.SamplesPerSec << endl;
    cout << "Number of bits used        :" << wav::wavHeader.bitsPerSample << endl;
    cout << "Number of channels         :" << wav::wavHeader.NumOfChan << endl;
    cout << "Number of bytes per second :" << wav::wavHeader.bytesPerSec << endl;
    cout << "Data length                :" << wav::wavHeader.Subchunk2Size << endl;
    cout << "Audio Format               :" << wav::wavHeader.AudioFormat << endl;
    ///< Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    cout << "Block align                :" << wav::wavHeader.blockAlign << endl;
    cout << "Data string                :" << wav::wavHeader.Subchunk2ID[0] << wav::wavHeader.Subchunk2ID[1] << wav::wavHeader.Subchunk2ID[2] << wav::wavHeader.Subchunk2ID[3] << endl;
}

void wav::setSubchunk2Size(uint32_t val){
    wavHeader.Subchunk2Size = val;
}

void wav::setBytesPerSec(uint32_t val){
    wavHeader.bytesPerSec = val;
}

void wav::setSamplesPerSec(uint32_t val){
    wavHeader.SamplesPerSec = val;
}

void wav::setChunkSize(uint32_t val){
    wavHeader.ChunkSize = val;
}

void wav::setNumOfChannels(uint16_t val){
    wavHeader.NumOfChan = val;
}

void wav::setBitsPerSample(uint16_t val){
    wavHeader.bitsPerSample = val;
}

void wav::setAudioFormat(uint16_t val){
    wavHeader.AudioFormat = val;
}

void wav::setBlockAlign(uint16_t val){
    wavHeader.blockAlign = val;
}

void wav::setWavHeader(wav_hdr header) {
    wavHeader = header;
}

void wav::setWavData(vector<short> data) {
    wavData = data;
}

void wav::setFile(string name){
    fileName = name;
}

void wav::setBytesPerSample(uint16_t val){
    bytesPerSample = val;
}

void wav::setNumSamples(uint32_t val){
    numSamples = val;
}

void wav::setTotalSamples(uint32_t val){
    totalSamples = val;
}

//< Getters
uint32_t wav::getSubchunk2Size(){
    return wavHeader.Subchunk2Size;
}

uint32_t wav::getBytesPerSec(){
    return wavHeader.bytesPerSec;
}

uint32_t wav::getSamplesPerSec(){
    return wavHeader.SamplesPerSec;
}

uint32_t wav::getChunkSize(){
    return wavHeader.ChunkSize;
}

uint16_t wav::getNumOfChannels(){
    return wavHeader.NumOfChan;
}

uint16_t wav::getBitsPerSample(){
    return wavHeader.bitsPerSample;
}

uint16_t wav::getAudioFormat(){
    return wavHeader.AudioFormat;
}

uint16_t wav::getBlockAlign(){
    return wavHeader.blockAlign;
}

wav_hdr wav::getWavHeader(){
    return wavHeader;
}

vector<short> wav::getWavData(){
    return wavData;
}

string wav::getFile() {
    return fileName;
}

uint16_t wav::getBytesPerSample(){
    return bytesPerSample;
}

uint32_t wav::getNumSamples(){
    return numSamples;
}

uint32_t wav::getTotalSamples(){
    return totalSamples;
}
