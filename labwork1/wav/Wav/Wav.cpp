///<
///< Created by diego on 03-10-2019.
///<

#include "Wav.h"
#include <fstream>

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
{   int headerSize = sizeof(wav_hdr);
    fwrite(&(Wav::wavHeader), 1, (size_t) headerSize, outwavfile);
    cout << "header done"<<endl;
    for(unsigned int i=0; i<Wav::wavHeader.Subchunk2Size; i+=Wav::bytesPerSample)
    {   signed char *p = (Wav::wavData) + i;
        fwrite(p,1,size_t (Wav::bytesPerSample), outwavfile);
    }
}

/**
 * Split wav channels into separate wav channels.
 * */
void Wav::splitChannels() {
    for(int i=0; i< wavHeader.NumOfChan; i++)
    {   auto *p = (signed char *) malloc(Wav::wavHeader.Subchunk2Size / Wav::wavHeader.NumOfChan);
        Wav::data_channels.push_back(p);
    }

    int totalSampleLen  = Wav::bytesPerSample*Wav::wavHeader.NumOfChan;
    for(unsigned int i=0; i< Wav::wavHeader.Subchunk2Size; i+=totalSampleLen) {
        signed char *p = Wav::wavData + i;
        int shift = (i / totalSampleLen) * Wav::bytesPerSample;
        for (unsigned long j = 0; j < Wav::wavHeader.NumOfChan; j++)
            memcpy((Wav::data_channels.at(j)) + shift, p + (j * Wav::bytesPerSample), (size_t) Wav::bytesPerSample);
    }
}

/**
 * Frees allocated memory for the each channel's sample's data.
 * */
void Wav::freeChannels()
{   if(!Wav::data_channels.empty())
    {   for(signed char *it:Wav::data_channels)
            free(it);
    }
}

/**
 * Records Channel data sample into specified wav file.
 *
 * @param chn channel to record channel's sample's data.
 * @param outwavfile file to store channel's sample's data.
 *
 * @return operation status
 * */
int Wav::recordChannel(uint16_t chn, FILE *outwavfile)
{   if(Wav::data_channels.empty())
        Wav::splitChannels();

    if(chn >Wav::wavHeader.NumOfChan || chn <= 0)
        return -1;

    wav_hdr tmpWavHeader    = Wav::wavHeader;

    tmpWavHeader.Subchunk2Size  = Wav::wavHeader.Subchunk2Size/Wav::wavHeader.NumOfChan;
    tmpWavHeader.bytesPerSec    = Wav::wavHeader.bytesPerSec/Wav::wavHeader.NumOfChan;
    tmpWavHeader.ChunkSize  =  Wav::wavHeader.ChunkSize - tmpWavHeader.Subchunk2Size;
    tmpWavHeader.NumOfChan  = 1;

    fwrite(&tmpWavHeader, 1, (size_t) sizeof(wav_hdr), outwavfile);
    fwrite(Wav::data_channels.at((unsigned long) chn-1),1, tmpWavHeader.Subchunk2Size ,outwavfile);

    return 1;
}

/**
 * Plots sample into a graph. Note samples values are parsed from -1 to 1.
 *
 * */
/**
//TODO: Find appropriate library for C++ to plot graphs
void Wav::plotSampling()
{   FILE* inwavfile   = fopen(Wav::filePath.c_str(),"r");
    wav_hdr header;
    int headerSize = sizeof(wav_hdr);
    size_t bytesRead = fread(&header, 1, (size_t) headerSize, inwavfile);
    signed char buffer[(header.bitsPerSample/8)*header.NumOfChan];
    int depth = header.Subchunk2Size/4;

    vector<double> data_channels[header.NumOfChan];
    for( auto it:data_channels)
        it = *(new vector<double >());


    while(!feof(inwavfile))
    {   fread(buffer, 1, (size_t ) (header.bitsPerSample/8)*header.NumOfChan, inwavfile);
        for(int i=0; i< header.NumOfChan; i++)
        {   signed int samp = ((((0xf00 & buffer[i*2+1]) == 0xf00)? 0xffff0000 : 0x00000000) | (((0xff & buffer[i*2+1]) << 8) | (0xff & buffer[i*2])));
            double fs_scale = ((double) samp/ 0xffff);
            data_channels[i].push_back(fs_scale);
        }
    }
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
}**/


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

    int nbytes = nbits/8;
    char mask= ~char(pow(2,nbits % 8)-1);
    auto finalData  = (signed char*) alloca(Wav::wavHeader.Subchunk2Size);

    for(unsigned int i=0; i< Wav::wavHeader.Subchunk2Size; i+=Wav::bytesPerSample)
    {   signed char *ptmp  = Wav::wavData + i;
        signed char *pfin  = finalData + i;
        int j =0;
        for(;j<nbytes; j++)
            pfin[j] = 0x00;

        pfin[j] = (ptmp[j] & mask);
        for(j++; j<Wav::bytesPerSample;j++)
            pfin[j] = ptmp[j];
    }
    fwrite(&(Wav::wavHeader), 1, (size_t) sizeof(wav_hdr), outfile);
    fwrite(finalData, 1, (size_t) Wav::wavHeader.Subchunk2Size, outfile);
    return 1;
}

int Wav::midriseQuant(int nbits, signed char* p, int size)
{   if(Wav::wavHeader.bitsPerSample < nbits)
    {   cout << "Can't operate: nbits > numper of bits per sample.";
        return -1;
    }
    int nbytes = nbits/8;
    char mask= ~char(pow(2,nbits % 8)-1);

    for(int i=0; i< size; i+=Wav::bytesPerSample)
    {   signed char *ptmp  = Wav::wavData + i;
        signed char *pfin  = p + i;
        int j =0;
        for(;j<nbytes; j++)
            pfin[j] = 0x00;

        pfin[j] = (ptmp[j] & mask);
        for(j++; j<Wav::bytesPerSample;j++)
            pfin[j] = ptmp[j];
    }
    return 1;
}

int Wav::midtreadQuant(int nbits, signed char* p, int size)
{   if(Wav::wavHeader.bitsPerSample < nbits)
    {   cout << "Can't operate: nbits > numper of bits per sample.";
        return 0;
    }
    if(nbits == 1)
        return Wav::midriseQuant(nbits, p, size);

    auto middelta = (signed short) (pow(2, nbits) / 2);
    auto maxVal =   (unsigned short) pow(2, Wav::wavHeader.bitsPerSample -1) -1;
    auto threshold = (unsigned short) pow(2, Wav::wavHeader.bitsPerSample -1) -1 - middelta;
    short mask_short = ~short(pow(2,nbits%16)-1);

    auto initNegSampVal = short (0x8000);
    if (Wav::bytesPerSample == 1)
        initNegSampVal = short (0xff80);

    for(unsigned int i=0; i< Wav::wavHeader.Subchunk2Size; i+=Wav::bytesPerSample)
    {   signed char *ptmp  = Wav::wavData + i;
        signed char *pfin  = p + i;

        auto samp = (signed short)(((0x80 & ptmp[Wav::bytesPerSample -1]) == 0x80)? initNegSampVal : 0x0000);
        for(int j=0; j <Wav::bytesPerSample; j++)
            samp |= (0xff & ptmp[j])<<(j*8);

        samp = (threshold<samp)? short(maxVal): ((samp + middelta) & mask_short);
        if (Wav::bytesPerSample == 2)
            pfin[1] = char((0xff00 & samp) >> 8);

        pfin[0] = char(0x00ff & samp);
    }
    return 1;
}

int Wav::encMidtreadUniQuant(int nbits, FILE *outfile)
{   if(Wav::wavHeader.bitsPerSample < nbits)
    {   cout << "Can't operate: nbits > numper of bits per sample.";
        return 0;
    }
    if(nbits == 1)
       return Wav::encMidriseUniQuant(nbits,outfile);

    auto middelta = (signed short) (pow(2, nbits) / 2);
    auto threshold = (unsigned short) pow(2, Wav::wavHeader.bitsPerSample -1) -1 - middelta;
    short mask_short = ~short(pow(2,nbits%16)-1);
    auto finalData  = (signed char*) alloca(Wav::wavHeader.Subchunk2Size);

    for(unsigned int i=0; i< Wav::wavHeader.Subchunk2Size; i+=Wav::bytesPerSample)
    {   signed char *ptmp  = Wav::wavData + i;
        signed char *pfin  = finalData + i;

        auto samp = (signed short)(((0x80 & ptmp[Wav::bytesPerSample -1]) == 0x80)? 0x8000 : 0x0000);
        for(int j=0; j <Wav::bytesPerSample; j++)
            samp |= (0xff & ptmp[j])<<(j*8);

        samp = (threshold<samp)? short(0x7fff): ((samp + middelta) & mask_short);
        if (Wav::bytesPerSample == 2) {
            pfin[1] = char((0xff00 & samp) >> 8);
            pfin[0] = char(0x00ff & samp);
        }
        else{
            pfin[0] = char(0x00ff & samp);
        }

    }
    fwrite(&(Wav::wavHeader), 1, (size_t) sizeof(wav_hdr), outfile);
    fwrite(finalData, 1, (size_t) Wav::wavHeader.Subchunk2Size, outfile);
    return 1;
}

double  Wav::getSNR(char typeQuant, int nbits, int chn, int& maxAbsError)
{   vector<double> xi;
    vector<double> xiQuant;
    double ES = 0,ER = 0, tmp;
    maxAbsError = 0;

    int N = Wav::numSamples / Wav::wavHeader.NumOfChan;
    auto* pQuant = (signed char*) malloc(Wav::wavHeader.Subchunk2Size);
    int ch_index = chn -1;
    int shift = Wav::bytesPerSample*Wav::wavHeader.NumOfChan + ch_index*Wav::bytesPerSample;
    signed char *pData;
    signed char *pQuantTmp;

    if (typeQuant == MIDRISE_QUANT)
    {   if(!Wav::midriseQuant(nbits, pQuant, Wav::wavHeader.Subchunk2Size))
        {   cout << "Could not quantize file." << endl;
            return 0;
        }
    }
    else if(typeQuant == MIDTREAD_QUANT)
    {   if(!Wav::midtreadQuant(nbits, pQuant, Wav::wavHeader.Subchunk2Size))
        {   cout << "Could not quantize file." << endl;
            return 0;
        }
    }
    else{
        cout << "Undefined type" << endl;
        return 0;
    }

    for(unsigned int i=0; i<Wav::wavHeader.Subchunk2Size; i+=shift) {
        pData = Wav::wavData + i;
        pQuantTmp = pQuant + i;

        auto sampOrigValue = (signed short)(((0x80 & pData[Wav::bytesPerSample -1]) == 0x80)? 0x8000 : 0x0000);
        auto sampQuantValue = (signed short)(((0x80 & pQuantTmp[Wav::bytesPerSample -1]) == 0x80)? 0x8000 : 0x0000);
        for(int k=0; k <Wav::bytesPerSample; k++)
        {   sampOrigValue   |= (0xff & pData[k])<<(k*8);
            sampQuantValue  |= (0xff & pQuantTmp[k])<<(k*8);
        }
        xi.push_back(sampOrigValue);
        xiQuant.push_back(sampQuantValue);
    }
    free(pQuant);

    for(long i =0; i<N; i++)
    {   ES += std::pow(std::abs(xi.at((unsigned long) i)),2);
        tmp = std::abs(xi.at((unsigned long) i) - xiQuant.at((unsigned long) i));
        ER += std::pow(tmp,2);
        if(maxAbsError<tmp)
            maxAbsError = tmp;
    }
    ES *= (double) 1/N;
    ER *= (double) 1/N;
    cout << "Maximum Absolute Error:\t"<< maxAbsError << endl;
    return 10*std::log10(ES/ER);
}

/**
 * Parses Wav file header information.
 * Reference: http:///<www.topherlee.com/software/pcm-tut-wavformat.html
 * */
void Wav::readHeader()
{   cout << "File is                    :" << getFileSize() << " bytes." << endl;
    cout << "RIFF header                :" << Wav::wavHeader.RIFF[0] << Wav::wavHeader.RIFF[1] << Wav::wavHeader.RIFF[2] << Wav::wavHeader.RIFF[3] << endl;
    cout << "WAVE header                :" << Wav::wavHeader.WAVE[0] << Wav::wavHeader.WAVE[1] << Wav::wavHeader.WAVE[2] << Wav::wavHeader.WAVE[3] << endl;
    cout << "FMT                        :" << Wav::wavHeader.fmt[0] << Wav::wavHeader.fmt[1] << Wav::wavHeader.fmt[2] << Wav::wavHeader.fmt[3] << endl;
    cout << "Data size                  :" << Wav::wavHeader.ChunkSize << endl;

    ///< Display the sampling Rate from the header
    cout << "Sampling Rate              :" << Wav::wavHeader.SamplesPerSec << endl;
    cout << "Number of bits used        :" << Wav::wavHeader.bitsPerSample << endl;
    cout << "Number of channels         :" << Wav::wavHeader.NumOfChan << endl;
    cout << "Number of bytes per second :" << Wav::wavHeader.bytesPerSec << endl;
    cout << "Data length                :" << Wav::wavHeader.Subchunk2Size << endl;
    cout << "Audio Format               :" << Wav::wavHeader.AudioFormat << endl;
    ///< Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    cout << "Block align                :" << Wav::wavHeader.blockAlign << endl;
    cout << "Data string                :" << Wav::wavHeader.Subchunk2ID[0] << Wav::wavHeader.Subchunk2ID[1] << Wav::wavHeader.Subchunk2ID[2] << Wav::wavHeader.Subchunk2ID[3] << endl;
}