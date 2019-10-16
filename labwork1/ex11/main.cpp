#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <vector>
#include <numeric>
#include <math.h>
#include "gnuplot.h"


using namespace cv;
using namespace std;

#define CHANNEL_R 2
#define CHANNEL_G 1
#define CHANNEL_B 0

#define CV_CAP_PROP_POS_FRAMES 1
#define CV_CAP_PROP_FRAME_COUNT 7

#define PLOT_R 2
#define PLOT_G 1
#define PLOT_B 0
#define PLOT_MERGE 3
#define PLOT_ALL 4

template <class InputIterator,class Functor>
std::vector<typename std::iterator_traits<InputIterator>::value_type>
filter( InputIterator begin, InputIterator end, Functor f )
{
    using ValueType = typename std::iterator_traits<InputIterator>::value_type;
    std::vector<ValueType> result;
    result.reserve( std::distance( begin, end ) );
    std::copy_if( begin, end, std::back_inserter( result ),f);
    return result;
}

double getChannelEntropy(Mat image, int channel)
{   if(!image.data)
    {   cout << "Could not open or find the image." << endl;
        return -1;
    }
    Vec3b intensity, prevNeighIntensity;
    std::map<pair<int,int>, double> chnl;
    map<int, double> entropyChnl;
    double chnlTotal, prob, meanEntropyCh;
    double min, max;

    vector<Mat> imageChnl;
    split(image,imageChnl);
    minMaxIdx(imageChnl[channel],&min, &max);

    for(int i=0; i<image.rows; i++)
    {   for(int j=1; j<image.cols; j++)
        {   intensity = image.at<Vec3b>(i, j);
            prevNeighIntensity = image.at<Vec3b>(i,j-1);
            chnl[make_pair(prevNeighIntensity.val[channel],intensity.val[channel])] ++;
        }
    }

    for(int i=(int) min; i<=max; i++)
    {   auto chnlFilt = filter( chnl.begin(), chnl.end(), [&i] (pair<const pair<int,int>,double> p) { return p.first.first == i;} );

        chnlTotal = std::accumulate(std::begin(chnlFilt), std::end(chnlFilt), 0,
                                  [](const std::size_t previous, const std::pair<pair<int,int>, double>& p)
                                  { return previous + p.second; });
        for(auto it: chnlFilt)
        {   prob = it.second/chnlTotal;
            entropyChnl[i] += prob*log2(prob);
        }
        entropyChnl[i] *= -1;
    }
    meanEntropyCh = (double) std::accumulate(std::begin(entropyChnl), std::end(entropyChnl), 0,
                                   [](const std::size_t previous, const std::pair<int,double>& p)
                                   { return previous + p.second; }) / entropyChnl.size();
    return meanEntropyCh;
}


double getEntropy(Mat image)
{   double entropy_r = getChannelEntropy(image, CHANNEL_R);
    double entropy_g = getChannelEntropy(image, CHANNEL_G);
    double entropy_b = getChannelEntropy(image, CHANNEL_B);

    return (entropy_r + entropy_g + entropy_b)/3;
}

double getChannelBlindEntropy(Mat image, int channel)
{   if(!image.data)
    {
        cout << "Could not open or find the image." << endl;
        return 0;
    }

    Vec3b intensity;
    unordered_map<int, double> chnl;
    double entropy_chnl=0, prob;
    int total = image.cols * image.rows;

    for(int i=0; i<image.cols; i++)
    {   for(int j=0; j<image.rows; j++)
        {   intensity = image.at<Vec3b>(j,i);
            chnl[intensity.val[channel]]++;
        }
    }

    for(auto it:chnl) {
        prob = it.second / total;
        entropy_chnl += prob * std::log10(prob);
    }
    return -entropy_chnl;
}

double getBlindEntropy(Mat image)
{   double entropy_r = getChannelBlindEntropy(image, CHANNEL_R);
    double entropy_g = getChannelBlindEntropy(image, CHANNEL_G);
    double entropy_b = getChannelBlindEntropy(image, CHANNEL_B);

    return (entropy_r + entropy_g + entropy_b)/3;

}

double videoEntropy()
{   VideoCapture cap("/home/diego/Workspace/ECT/5ano/CAV/project/cav-2019/labwork1/files/video/crew_cif.y4m");
    if(! cap.isOpened())
        return -1;

    namedWindow("video",1);
    double frnb(cap.get(CV_CAP_PROP_FRAME_COUNT));
    std::cout << "frame count = " << frnb << endl;
    map<int, double> entropyValues;
    for(int i =0;i<frnb;i++)
    {
        cout << "\r\e[K"<< to_string((int)((i/frnb)*100))<<"%"<<flush;

        Mat frame;
        cap.set(CV_CAP_PROP_POS_FRAMES, i);
        if(!cap.read(frame))
        {   cout << "Cannot read frame" << endl;
            break;
        }
        entropyValues[i]=getEntropy(frame);
    }
    cout<< endl;
    GnuplotPipe gp;
    gp.sendLine("set title 'Entropia'");
    gp.sendLine("set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2 pointtype 7 pointsize 0.1 ");
    gp.sendLine("plot 'merge channel' with linespoints linestyle 1");

    for(auto it = entropyValues.cbegin(); it != entropyValues.cend(); ++it)
        gp.sendLine(std::to_string(it->first) + " " + std::to_string(it->second));

    gp.sendEndOfData();
}




int main(int argc, char** argv) {
    if(argc != 2)
    {
        cout << "Usage: ex11 ImageToLoadAndDisplay" << endl;
        return -1;
    }
    //GnuplotPipe gp;
    //gp.sendLine("plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))");
    videoEntropy();

    return 0;
    Mat image;
    image   = imread(argv[1], 1);

    cout << "Entropy R:\t"<< getChannelEntropy(image, CHANNEL_R)<<endl;
    cout << "Entropy G:\t"<< getChannelEntropy(image, CHANNEL_G)<<endl;
    cout << "Entropy B:\t"<< getChannelEntropy(image, CHANNEL_B)<<endl;
    cout << "Entopy:\t"<< getEntropy(image)<<endl;
    cout << endl;

    cout << "Blind Entropy R:\t"<< getChannelBlindEntropy(image, CHANNEL_R)<<endl;
    cout << "Blind Entropy G:\t"<< getChannelBlindEntropy(image, CHANNEL_G)<<endl;
    cout << "Blind Entropy B:\t"<< getChannelBlindEntropy(image, CHANNEL_B)<<endl;
    cout << "Blind Entopy:\t"<< getBlindEntropy(image)<<endl;

    return 0;
}