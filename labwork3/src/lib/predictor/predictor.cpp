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

    /*switch(mode) {
        case 1:
            if (ptr == 0)
                return ret; 
            // A
            ret = short(mat[ptr - 1]);
            break;
        case 2:
            if (ptr < width)
                return ret; 
            // B
            ret = short(mat[ptr - width]);
            break;
        case 3:
            if (ptr < (width - 1))
                return ret; 
            // C
            ret = short(mat[ptr - width - 1]);
            break;
        case 4:
            if (ptr < (width - 1))
                return ret; 
            // A+B-C
            ret = short(mat[ptr - 1]) + short(mat[ptr - width]) - short(mat[ptr - width - 1]);
            break;
        case 5:
            if (ptr < (width - 1))
                return ret; 
            // A+(B-C)/2
            ret = short(mat[ptr - 1]) + (short(mat[ptr - width]) - short(mat[ptr - width - 1]))/2;
            break;
        case 6:
            if (ptr < (width - 1))
                return ret; 
            // B+(A-C)/2
            ret = short(mat[ptr - width]) + (short(mat[ptr - 1]) - short(mat[ptr - width - 1]))/2;
            break;
        case 7:
            if (ptr < (width - 1))
                return ret; 
            // (A+B)/2
            ret = (short(mat[ptr - 1]) + short(mat[ptr - width]))/2;
            break;
    }*/
    return ret;
}

short predictor::calcResidual(ushort x, ushort y, uint8_t mode) {
    return short(origMat->at<uchar>(x, y)) - predict(x, y, mode);
}

void predictor::calcBlockResiduals(ushort x, ushort y, uint8_t mode, cv::Mat* resMat) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            resMat->at<short>(i, j) = calcResidual(x+i, y+j, mode);
        }
    }
}

void predictor::reconstruct(ushort x, ushort y, uint8_t mode, short residual) {
    origMat->at<uchar>(x,y) = predict(x, y, mode) + residual;
}

void predictor::reconstructBlock(ushort x, ushort y, uint8_t mode, cv::Mat* resMat) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            reconstruct(x+i, y+j, mode, resMat->at<short>(i, j));
        }
    }
}
