//
// Created by diego on 17/11/19.
//

#include "wav.h"


wav::wav(string filename)
{   char* ptrData;
    int headerSize = sizeof(wav_hdr);
    ptrData = (char*) malloc(wavHeader.Subchunk2Size);

    FILE* inwavfile   = fopen(filename.c_str(),"r");
    fread(&wavHeader, 1, (size_t) headerSize, inwavfile);
    fread(ptrData, 1, wavHeader.Subchunk2Size, inwavfile);
    fclose(inwavfile);

    bytesPerSample = wavHeader.bitsPerSample/8;
    numSamples = wavHeader.Subchunk2Size/bytesPerSample;
    vector<short> tmpData(numSamples);
    string tmp;


    for(signed int i = 0, j = 0; i< wavHeader.Subchunk2Size; i+=bytesPerSample, j++)
    {   char *pchar  = ptrData + i;
        short samp =  (((short)pchar[1])<<8) | (0x00ff & pchar[0]);
        //wavData.push_back(samp);
        tmpData[j] = samp;
    }
    wav::wavData = tmpData;
    free(ptrData);
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

wav_hdr wav::getWavHeader()
{   return wavHeader;
}

vector<short> wav::getWavData(){
    return wavData;
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
