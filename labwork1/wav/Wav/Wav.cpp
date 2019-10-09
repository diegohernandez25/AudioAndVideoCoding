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
    fwrite(&wavHeader, 1, (size_t) headerSize, outwavfile);
    char buffer[wavHeader.bitsPerSample];
    while(!feof(inwavfile))
    {   fread(buffer, 1, (size_t ) wavHeader.bitsPerSample, inwavfile);
        fwrite(buffer, 1, wavHeader.bitsPerSample, outwavfile);
    }
    fflush(outwavfile);
}

/**
 * Split wav channels into separate wav channels.
 *
 * */
void Wav::splitChannels() {
    FILE* inwavfile = fopen(Wav::filePath.c_str(),"r");
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr);
    size_t bytesRead = fread(&wavHeader, 1, (size_t) headerSize, inwavfile);

    for(int i=0; i< wavHeader.NumOfChan; i++)
    {   char *p = (char *) malloc(sizeof(signed char) * (Wav::wavHeader.Subchunk2Size/Wav::wavHeader.NumOfChan));
        cout << "allocated space for pointer: \t"<<sizeof(signed char) * (Wav::wavHeader.Subchunk2Size/Wav::wavHeader.NumOfChan)<< endl;
        Wav::data_channels.push_back(p);
    }


    int numCharSamples = 0;
    signed char buffer[(wavHeader.bitsPerSample/8)*wavHeader.NumOfChan];
    while(!feof(inwavfile))
    {   fread(buffer, wavHeader.bitsPerSample/8, (size_t ) wavHeader.NumOfChan, inwavfile);
        for(unsigned long i=0; i< wavHeader.NumOfChan; i++) {
            memcpy(Wav::data_channels.at(i) + numCharSamples, buffer + i * wavHeader.bitsPerSample/8, wavHeader.bitsPerSample/8);
        }
        numCharSamples += wavHeader.bitsPerSample/8;
    }
    cout << "numCharSamples:\t" << to_string(numCharSamples)<< endl;
    fclose(inwavfile);
}

int Wav::recordChannel(uint16_t chn, FILE *outwavfile)
{   if(Wav::data_channels.empty())
        Wav::splitChannels();

    if(chn > Wav::wavHeader.NumOfChan || chn < 0)
    {   cout << "Channel selection out of range." << endl;
        return -1;
    }

    wav_hdr tmpWavHeader    = Wav::wavHeader;
    tmpWavHeader.NumOfChan  = 1;
    tmpWavHeader.Subchunk2Size  = Wav::wavHeader.Subchunk2Size/Wav::wavHeader.NumOfChan;
    tmpWavHeader.ChunkSize  =  Wav::wavHeader.ChunkSize - tmpWavHeader.Subchunk2Size;
    tmpWavHeader.bytesPerSec    = Wav::wavHeader.bytesPerSec/Wav::wavHeader.NumOfChan;

    fwrite(&tmpWavHeader, 1, (size_t) sizeof(wav_hdr), outwavfile);
    fwrite(Wav::data_channels.at(chn),1, tmpWavHeader.Subchunk2Size ,outwavfile);
    return 1;
}

/**
 * Plots sample into a graph. Note samples values are parsed from -1 to 1.
 * //TODO: Instead of reading the file, use classes atributes values.
 * */
void Wav::plotSampling()
{   FILE* inwavfile   = fopen(Wav::filePath.c_str(),"r");
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr);
    signed char buffer[(wavHeader.bitsPerSample/8)*wavHeader.NumOfChan];

    vector<double> data_channels[wavHeader.NumOfChan];
    for( auto it:data_channels)
        it = *(new vector<double >());


    int numCharPerSample = 0;
    while(!feof(inwavfile))
    {   fread(buffer, 1, (size_t ) (wavHeader.bitsPerSample/8)*wavHeader.NumOfChan, inwavfile);
        for(int i=0; i< wavHeader.NumOfChan; i++)
        {   signed int samp = ((((0xf00 & buffer[i*2+1]) == 0xf00)? 0xffff0000 : 0x00000000) | (((0xff & buffer[i*2+1]) << 8) | (0xff & buffer[i*2])));
            double fs_scale = ((double) samp/ 0xffff);
            data_channels[i].push_back(fs_scale);
            numCharPerSample+=2;
        }
    }

    fclose(inwavfile);
    int channel = 0;
    for(auto it : data_channels)
    {   Mat data(it);
        Ptr<plot::Plot2d> plot = plot::Plot2d::create(data);
        while( true ){
            double value = *it.begin();
            it.erase(it.begin());
            it.push_back(value);
            Mat image;
            plot->render(image);
            imshow("Channel "+to_string(channel), image);
            if( waitKey( 33 ) >= 0 )
                break;

        }
        channel++;
    }
    destroyAllWindows();
}

/**
 * Encode Quantize Wav file using Midrise approach
 *
 * @param nbits number of bits to remove.
 * @param outfile file to write quantize wav data.
 *
 * @return operation status.
 * */
int Wav::encMidriseUniQuant(int nbits, FILE *outfile)
{   if(Wav::wavHeader.bitsPerSample < nbits)
    {   cout << "Can't operate: nbits > numper of bits per sample.";
        return -1;
    }
    int nbytes  = nbits/8;
    char mask= ~char(pow(2,nbits%8)-1);

    int sampleSize  = Wav::wavHeader.bitsPerSample/8;
    auto finalData  = (signed char*) alloca(Wav::wavHeader.Subchunk2Size);

    for(int i=0; i< Wav::wavHeader.Subchunk2Size; i+=sampleSize)
    {   signed char *ptmp  = Wav::wavData + i;
        signed char *pfin  = finalData + i;
        int k =0;
        for(;k<nbytes; k++)
            pfin[k] = 0x00;

        pfin[k] = ptmp[k] & mask;

        for(; k<sampleSize; k++)
            pfin[k] = ptmp[nbytes+k];
    }
    fwrite(&(Wav::wavHeader), 1, (size_t) sizeof(wav_hdr), outfile);
    fwrite(finalData, 1, (size_t) Wav::wavHeader.Subchunk2Size, outfile);
    return 1;
}


int Wav::encMidtreadUniQuant(int nbits, FILE *outfile)
{   if(Wav::wavHeader.bitsPerSample < nbits)
    {   cout << "Can't operate: nbits > numper of bits per sample.";
        return -1;
    }

    if(nbits == 1)
    {   Wav::encMidriseUniQuant(nbits,outfile);
        return 1;
    }


}



/**
 * Parses Wav file header information.
 * Reference: http:///<www.topherlee.com/software/pcm-tut-wavformat.html
 * */
void Wav::readHeader()
{   FILE* wavFile   = fopen(Wav::filePath.c_str(),"r");
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr);
    cout << "Size of header:\t" << to_string(headerSize) << endl;
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