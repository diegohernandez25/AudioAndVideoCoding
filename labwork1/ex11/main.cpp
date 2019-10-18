#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <tuple>

#include "gnuplot.h"


#define CHANNEL_R 2
#define CHANNEL_G 1
#define CHANNEL_B 0

#define CV_CAP_PROP_POS_FRAMES 1
#define CV_CAP_PROP_FRAME_COUNT 7

#define PLOT_R 2
#define PLOT_G 1
#define PLOT_B 0
#define PLOT_MERGE 3
#define PLOT_ALL_RGB 4
#define PLOT_ALL 5

using namespace cv;
using namespace std;

/**
 * Filter function.
 *
 * @param begin Iterator starting point.
 * @param end Iterator ending point.
 * @param f lambda function to operate.
 *
 * @return elements that are according to the given lambda function criteria.
 * */
template <class InputIterator,class Functor>
std::vector<typename std::iterator_traits<InputIterator>::value_type>
filter( InputIterator begin, InputIterator end, Functor f )
{   using ValueType = typename std::iterator_traits<InputIterator>::value_type;
    std::vector<ValueType> result;
    result.reserve( std::distance( begin, end ) );
    std::copy_if( begin, end, std::back_inserter( result ),f);
    return result;
}

/**
 *  Get image channel entropy.
 *
 *  @param image image frame.
 *  @param channel channel number. CHANNEL_R, CHANNEL_G, CHANNEL_B.
 *
 *  @return Image's channel's entropy value.
 * */
double getChannelEntropy(Mat image, int channel)
{   if(!image.data)
    {   cout << "Could not open or find the image." << endl;
        return -1;
    }
    Vec3b intensity, prevNeighIntensity;
    std::map<pair<int,int>, double> chnl;
    map<int, double> entropyChnl;
    double chnlTotal, prob, EntropyCh;
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
    EntropyCh = (double) std::accumulate(std::begin(entropyChnl), std::end(entropyChnl), 0,
                                   [](const std::size_t previous, const std::pair<int,double>& p)
                                   { return previous + p.second; }) / entropyChnl.size();
    return EntropyCh;
}


/**
 *  Get image entropy. It takes into account the all channel's entropy values of the image and calculate its mean.
 *
 *  @param image image frame.
 *  @param entropy_r pointer type double to store 'R' channel entropy value of the image.
 *  @param entropy_g pointer type double to store 'G' channel entropy value of the image.
 *  @param entropy_b pointer type double to store 'B' channel entropy value of the image.
 *
 *  @return Image's entropy value.
 * */
double getEntropy(Mat image, double& entropy_r, double& entropy_g, double& entropy_b)
{   entropy_r = getChannelEntropy(image, CHANNEL_R);
    entropy_g = getChannelEntropy(image, CHANNEL_G);
    entropy_b = getChannelEntropy(image, CHANNEL_B);

    return (entropy_r + entropy_g + entropy_b)/3;
}

/**
 *  Get image channel blind entropy.
 *
 *  @param image image frame.
 *  @param channel channel number. CHANNEL_R, CHANNEL_G, CHANNEL_B.
 *
 *  @return Image's channel's blind entropy value.
 * */
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
        entropy_chnl += prob * std::log2(prob);
    }
    return -entropy_chnl;
}

/**
 *  Get image blind entropy. It takes into account the all channel's entropy values of the image and calculate its mean.
 *
 *  @param image image frame.
 *  @param entropy_r pointer type double to store 'R' channel blind entropy value of the image.
 *  @param entropy_g pointer type double to store 'G' channel blind entropy value of the image.
 *  @param entropy_b pointer type double to store 'B' channel blind entropy value of the image.
 *
 *  @return Image's blind entropy value.
 * */
double getBlindEntropy(const Mat& image, double& entropy_r, double& entropy_g, double& entropy_b)
{   entropy_r = getChannelBlindEntropy(image, CHANNEL_R);
    entropy_g = getChannelBlindEntropy(image, CHANNEL_G);
    entropy_b = getChannelBlindEntropy(image, CHANNEL_B);
    return (entropy_r + entropy_g + entropy_b)/3;
}

/**
 * Visualize through gnuplot the plot that represents each entropy value of a video's image frame.
 *
 * @param plotType type of plot to visualize. PLOT_R, PLOT_G, PLOT_B, PLOT_MERGE, PLOT_ALL_RGB, PLOT_ALL.
 * @param title title of the graph.
 * @param entropyValues map of all the entropy values of the video's frames.
 * @param entropyRValues map of all the entropy values from channel 'R' of the video's frames.
 * @param entropyGValues map of all the entropy values from channel 'G' of the video's frames.
 * @param entropyBValues map of all the entropy values from channel 'B' of the video's frames.
 * */
void plotEntropyChannels(int plotType, const string& title, const map<int, double>& entropyValues,
        const map<int, double>& entropyRValues,const map<int, double>& entropyGValues,
        const map<int, double>& entropyBValues)
{   GnuplotPipe gp;
    gp.sendLine("set title \"" + title + "\"");

    gp.sendLine("set xlabel \"Frame number.\"");
    gp.sendLine("set ylabel \"Frame entropy value.\"");

    gp.sendLine("set style line 1 linecolor rgb '#828293' linetype 1 linewidth 2 pointtype 7 pointsize 0.1");
    gp.sendLine("set style line 2 linecolor rgb '#df3635' linetype 1 linewidth 2 pointtype 7 pointsize 0.1");
    gp.sendLine("set style line 3 linecolor rgb '#41e591' linetype 1 linewidth 2 pointtype 7 pointsize 0.1");
    gp.sendLine("set style line 4 linecolor rgb '#2c7bef' linetype 1 linewidth 2 pointtype 7 pointsize 0.1");

    switch(plotType)
    {   case PLOT_ALL:
            gp.sendLine("plot '-' with linespoints linestyle 1 t \"Merge Channels\","
                        "'-' with linespoints linestyle 2 t \"Red\","
                        "'-' with linespoints linestyle 3 t \"Green\","
                        "'-' with linespoints linestyle 4 t \"Blue\"");
            break;

        case PLOT_ALL_RGB:
            gp.sendLine("plot '-' with linespoints linestyle 2 t \"Red\","
                        "'-' with linespoints linestyle 3 t \"Green\","
                        "'-' with linespoints linestyle 4 t \"Blue\"");
            break;

        case PLOT_R:
            gp.sendLine("plot '-' with linespoints linestyle 2 t \"Red\"");
            break;

        case PLOT_G:
            gp.sendLine("plot '-' with linespoints linestyle 2 t \"Green\"");
            break;

        case PLOT_B:
            gp.sendLine("plot '-' with linespoints linestyle 2 t \"Blue\"");
            break;
    }

    if(plotType == PLOT_MERGE || plotType == PLOT_ALL)
    {   for(auto it:entropyValues)
            gp.sendLine(std::to_string(it.first) + " " + std::to_string(it.second));
        gp.sendEndOfData();
    }

    if(plotType == PLOT_R|| plotType == PLOT_ALL || plotType == PLOT_ALL_RGB)
    {   for(auto it:entropyRValues)
            gp.sendLine(std::to_string(it.first) + " " + std::to_string(it.second));
        gp.sendEndOfData();
    }

    if(plotType == PLOT_G|| plotType == PLOT_ALL || plotType == PLOT_ALL_RGB)
    {   for(auto it:entropyGValues)
            gp.sendLine(std::to_string(it.first) + " " + std::to_string(it.second));
        gp.sendEndOfData();
    }

    if(plotType == PLOT_B|| plotType == PLOT_ALL || plotType == PLOT_ALL_RGB)
    {   for(auto it:entropyBValues)
            gp.sendLine(std::to_string(it.first) + " " + std::to_string(it.second));
        gp.sendEndOfData();
    }
}

/**
 * Calculates entropy of a video according to a given entropy method.
 *
 * @param cap video.
 * @param entropyFunc entropy method used to calculate the entropy if each video's frame.
 * The available functions are: getEntropy and getBlindEntropy.
 * @param entropyValues map of all the entropy values of the video's frames.
 * @param entropyRValues map of all the entropy values from channel 'R' of the video's frames.
 * @param entropyGValues map of all the entropy values from channel 'G' of the video's frames.
 * @param entropyBValues map of all the entropy values from channel 'B' of the video's frames.
 *
 * @return tuple containing the average values of: total entropy, entropy of channel 'R', entropy of channel 'G',
 * entropy of channel 'B'.
 * */
template <typename Function>
tuple<double ,double, double, double> videoEntropy(VideoCapture cap, Function entropyFunc,
        map<int, double> &entropyValues, map<int, double> &entropyRValues, map<int, double> &entropyGValues,
        map<int, double> &entropyBValues)
{   if(! cap.isOpened())
        return {NULL,NULL,NULL,NULL};

    namedWindow("video",1);
    double frnb(cap.get(CV_CAP_PROP_FRAME_COUNT));
    std::cout << "frame count : " << frnb << endl;

    double r, g, b, r_mean=0, g_mean=0, b_mean=0, t_mean=0;
    for(int i =0;i<frnb;i++)
    {   cout << "\r\e[K"<< to_string((int)((i/frnb)*100))<<"%"<<flush;
        Mat frame;
        cap.set(CV_CAP_PROP_POS_FRAMES, i);
        if(!cap.read(frame))
        {   cout << "Cannot read frame" << endl;
            break;
        }
        t_mean += entropyValues[i] = entropyFunc(frame,r,g,b);
        r_mean += entropyRValues[i] = r; g_mean += entropyGValues[i] = g; b_mean += entropyBValues[i] = b;

    }
    cout<<"\r\e[K100%"<<endl;
    return {t_mean/frnb, r_mean/frnb, g_mean/frnb, b_mean/frnb};
}

/**
 * Part 1 of exercise 11
 *
 * @param image_path path of the image file to analyse.
 * */
void ex11_part1(const string image_path)
{   Mat image;
    image   = imread(image_path, 1);
    double r,g,b;
    cout << "Entopy:\t"<< getEntropy(image,r,g,b)<<endl;
    cout << "Entropy R:\t"<< r <<endl;
    cout << "Entropy G:\t"<< g <<endl;
    cout << "Entropy B:\t"<< b <<endl;
    cout << endl;

    cout << "Blind Entopy:\t"<< getBlindEntropy(image,r,g,b)<<endl;
    cout << "Blind Entropy R:\t"<< r <<endl;
    cout << "Blind Entropy G:\t"<< g <<endl;
    cout << "Blind Entropy B:\t"<< b <<endl;
}

/**
 * Part 2 of exercise 11
 *
 * @param video_path path of the video file to analyse.
 * */
void ex11_part2(const string video_path)
{   VideoCapture cap(video_path);

    map<int, double> entropyValues, entropyRValues,entropyGValues,entropyBValues;
    tuple<double, double , double ,double > r_mean =videoEntropy(cap,getEntropy,entropyValues, entropyRValues, entropyGValues,entropyBValues);
    cout << "Total Video Entropy:\t"<< get<0>(r_mean)<<endl;
    cout << "Red Video Entropy:\t"<< get<1>(r_mean)<<endl;
    cout << "Green Video Entropy:\t"<<get<2>(r_mean)<<endl;
    cout << "Blue Video Entropy:\t"<<get<3>(r_mean)<<endl;
    plotEntropyChannels(PLOT_ALL,"Entropy", entropyValues, entropyRValues, entropyGValues, entropyBValues);

    entropyValues.clear(); entropyRValues.clear(); entropyGValues.clear(); entropyBValues.clear();

    r_mean =videoEntropy(cap, getBlindEntropy,entropyValues, entropyRValues, entropyGValues,entropyBValues);
    cout << endl;
    cout << "Total Video Blind Entropy:\t"<< get<0>(r_mean)<<endl;
    cout << "Red Video Blind Entropy:\t"<< get<1>(r_mean)<<endl;
    cout << "Green Video Blind Entropy:\t"<<get<2>(r_mean)<<endl;
    cout << "Blue Video Blind Entropy:\t"<<get<3>(r_mean)<<endl;
    plotEntropyChannels(PLOT_ALL,"Blind Entropy", entropyValues, entropyRValues, entropyGValues, entropyBValues);

}

int main(int argc, char** argv) {
    cout << "Ex11 part 1" << endl;
    cout << "-----------" << endl;
    cout << "Image file path: ";
    string path;
    getline(cin,path);
    ex11_part1(path);
    cout<<endl;
    cout << "Ex11 part 2" << endl;
    cout << "-----------" << endl;
    cout << "Video file path: ";
    getline(cin,path);
    ex11_part2(path);
    cout<<endl;

    cout << "End" << endl;
    return 0;
}