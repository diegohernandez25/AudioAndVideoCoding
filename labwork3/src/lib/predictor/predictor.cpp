#include "predictor.h"

#include <numeric>
#include <math.h>

using namespace std;

predictor::predictor(int mode, int width) {
    this->mode = mode;
    this->width = width;
    ptr = 0;
}

void predictor::setMat(uchar* mat) {
    this->mat = mat;
}

short predictor::predict() {
    short ret = 0;

    switch(mode) {
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
    }
    return ret;
}

int predictor::residual() {
    short pred_val = predict();
    int res = short(mat[ptr]) - pred_val;
    ptr++;
    return res;
}

void predictor::reconstruct(short residual) {
    mat[ptr] = predict() + residual;
    ptr++;
}
