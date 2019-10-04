///<
///< Created by diego on 03-10-2019.
///<

#include "Wav.h"

/**
 * Gets file size.
 *
 * @return fileSize the file size in bytes.
 * */
int Wav::getFileSize()
{   FILE* wavFile   = fopen(Wav::filePath.c_str(),"r");
    int fileSize = 0;
    fseek(wavFile, 0, SEEK_END);
    fileSize = ftell(wavFile);
    fseek(wavFile, 0, SEEK_SET);
    fclose(wavFile);
    return fileSize;
}

/**
 * Copy a Wav file to another Wav file sample by sample.
 *
 * @param outwavfile output wav file. File name must end with format reference '.wav'
 *
 * */
void Wav::cpBySample(FILE *outwavfile)
{   FILE* inwavfile   = fopen(Wav::filePath.c_str(),"r");
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr);
    size_t bytesRead = fread(&wavHeader, 1, (size_t) headerSize, inwavfile);
    cout << "read done"<< endl;
    fwrite(&wavHeader, 1, (size_t) headerSize, outwavfile);
    cout << "write done" << endl;
    char buffer[wavHeader.bitsPerSample];
    while(!feof(inwavfile))
    {   fread(buffer, 1, (size_t ) wavHeader.bitsPerSample, inwavfile);
        fwrite(buffer, 1, wavHeader.bitsPerSample, outwavfile);
    }
    fflush(outwavfile);
}



/**
 * Parses Wav file header information.
 * Reference: http:///<www.topherlee.com/software/pcm-tut-wavformat.html
 * */
void Wav::readHeader()
{   FILE* wavFile   = fopen(Wav::filePath.c_str(),"r");
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr);
    size_t bytesRead = fread(&wavHeader, 1, (size_t) headerSize, wavFile);
    int filelength = getFileSize();
    if(bytesRead > 0)
    {   cout << "File is                    :" << filelength << " bytes." << endl;
        cout << "RIFF header                :" << wavHeader.RIFF[0] << wavHeader.RIFF[1] << wavHeader.RIFF[2] << wavHeader.RIFF[3] << endl;
        cout << "WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3] << endl;
        cout << "FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3] << endl;
        cout << "Data size                  :" << wavHeader.ChunkSize << endl;
        ///< Display the sampling Rate from the header
        cout << "Sampling Rate              :" << wavHeader.SamplesPerSec << endl;
        cout << "Number of bits used        :" << wavHeader.bitsPerSample << endl;
        cout << "Number of channels         :" << wavHeader.NumOfChan << endl;
        cout << "Number of bytes per second :" << wavHeader.bytesPerSec << endl;
        cout << "Data length                :" << wavHeader.Subchunk2Size << endl;
        cout << "Audio Format               :" << wavHeader.AudioFormat << endl;
        ///< Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
        cout << "Block align                :" << wavHeader.blockAlign << endl;
        cout << "Data string                :" << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << endl;

    }
    fclose(wavFile);
}