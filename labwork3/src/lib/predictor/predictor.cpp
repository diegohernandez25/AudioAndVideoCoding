#include "predictor.h"

#include <numeric>
#include <math.h>

using namespace std;

predictor::predictor(ushort width, ushort height) {
    this->width = width;
    this->height = height;
}

void predictor::newFrame(cv::Mat* mat) {
    origMat = mat;
}

short predictor::predict(ushort x, ushort y, uint8_t mode) {
    short ret = 0;

    switch(mode) {
        case 1:
            if (x == 0) {
                return ret; 
            }
            // A
            ret = short(origMat->at<uchar>(y,x-1));
            break;
        case 2:
            if (y == 0)
                return ret; 
            // B
            ret = short(origMat->at<uchar>(y-1,x));
            break;
        case 3:
            if (x == 0 || y == 0)
                return ret; 
            // C
            ret = short(origMat->at<uchar>(y-1,x-1));
            break;
        case 4:
            if (x == 0 || y == 0)
                return ret; 
            // A+B-C
            ret = short(origMat->at<uchar>(y,x-1)) + short(origMat->at<uchar>(y-1,x)) - short(origMat->at<uchar>(y-1,x-1));
            break;
        case 5:
            if (x == 0 || y == 0)
                return ret; 
            // A+(B-C)/2
            ret = short(origMat->at<uchar>(y,x-1)) + (short(origMat->at<uchar>(y-1,x)) - short(origMat->at<uchar>(y-1,x-1)))/2;
            break;
        case 6:
            if (x == 0 || y == 0)
                return ret; 
            // B+(A-C)/2
            ret = short(origMat->at<uchar>(y-1,x)) + (short(origMat->at<uchar>(y,x-1)) - short(origMat->at<uchar>(y-1,x-1)))/2;
            break;
        case 7:
            if (x == 0 || y == 0)
                return ret; 
            // (A+B)/2
            ret = (short(origMat->at<uchar>(y,x-1)) + short(origMat->at<uchar>(y-1,x)))/2;
            break;
        case 8:
            if (x == 0 || y == 0)
                return ret; 
            
            // LOCO-I
            short a = short(origMat->at<uchar>(y,x-1));
            short b = short(origMat->at<uchar>(y-1,x));
            short c = short(origMat->at<uchar>(y-1,x-1));
            short minAB, maxAB;

            if (b < a) {
                minAB = b;
                maxAB = a;
            } else {
                minAB = a;
                maxAB = b;
            }

            if (c >= maxAB)
                ret = minAB;
            else if (c <= minAB)
                ret = maxAB;
            else
                ret = a + b - c;
            break;
    }
    if (ret < 0)
        ret = 0;
    if (ret > 255)
        ret = 255;
    return ret;
}

short predictor::calcResidual(ushort x, ushort y, uint8_t mode) {
    return short(origMat->at<uchar>(y,x)) - predict(x, y, mode);
}

void predictor::calcBlockResiduals(ushort x, ushort y, uint8_t mode, cv::Mat* resMat) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            resMat->at<short>(j, i) = calcResidual(x+i, y+j, mode);
        }
    }
}

// mode, score
tuple<uint8_t, uint> predictor::calcBestResiduals(ushort x, ushort y, cv::Mat* resMat) {
    uint8_t mode = 0;
    cv::Mat tmp(width, height, CV_16S);
    (*origMat)(cv::Rect_<uint>(x, y, width, height)).convertTo(tmp, CV_16S);
    tmp.copyTo(*resMat);
    int score = cv::sum(cv::abs(*resMat))[0];
    int tmpScore = 0;

    int numPreds = width > 16 ? 9 : 8;
    for (int i = 1; i < numPreds; i++) {
        calcBlockResiduals(x, y, i, &tmp);
        tmpScore = cv::sum(cv::abs(tmp))[0];
        if (tmpScore < score) {
            score = tmpScore;
            mode = i;
            tmp.copyTo(*resMat);
        }
    }
    return make_tuple(mode, score);
}

void predictor::reconstruct(ushort x, ushort y, uint8_t mode, short residual) {
    origMat->at<uchar>(y,x) = predict(x, y, mode) + residual;
}

void predictor::reconstructBlock(ushort x, ushort y, uint8_t mode, cv::Mat* resMat) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            reconstruct(x+i, y+j, mode, resMat->at<short>(j, i));
        }
    }
}
