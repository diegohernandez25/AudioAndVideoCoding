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
            if (x == 0)
                return ret; 
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
    }
    if (ret < 0)
        ret = 0;
    return ret;
}

short predictor::calcResidual(ushort x, ushort y, uint8_t mode) {
    return short(origMat->at<uchar>(y,x)) - predict(x, y, mode);
}

void predictor::calcBlockResiduals(ushort x, ushort y, uint8_t mode, cv::Mat* resMat) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            resMat->at<short>(i, j) = calcResidual(x+i, y+j, mode);
        }
    }
}

void predictor::reconstruct(ushort x, ushort y, uint8_t mode, short residual) {
    origMat->at<uchar>(y,x) = predict(x, y, mode) + residual;
}

void predictor::reconstructBlock(ushort x, ushort y, uint8_t mode, cv::Mat* resMat) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            reconstruct(x+i, y+j, mode, resMat->at<short>(i, j));
        }
    }
}
