#include "predictor.h"

#include <numeric>
#include <math.h>

using namespace std;

predictor::predictor(short width, short height) {
    this->width = width;
    this->height = height;
}

void predictor::newFrame(cv::Mat* mat) {
    origMat = mat;
}

short predictor::predict(short x, short y, uint8_t mode) {
    short ret = 0;

    // TODO migrate to cvmatrix
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

short predictor::calcResidual(short x, short y, uint8_t mode) {
    return short(origMat->at<uchar>(x, y)) - predict(x, y, mode);
}

void predictor::reconstruct(short x, short y, uint8_t mode, short residual) {
    origMat->at<uchar>(x,y) = predict(x, y, mode) + residual;
}
