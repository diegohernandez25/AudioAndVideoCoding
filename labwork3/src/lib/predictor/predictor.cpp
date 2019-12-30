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
    // TODO start with 0?
    short ret = 0;

    switch(mode) {
        case 1:
            if (ptr == 0)
                return ret; 
            ret = short(mat[ptr - 1]);
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
