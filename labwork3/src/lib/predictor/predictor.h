#ifndef PREDICTOR_PREDICTOR_H
#define PREDICTOR_PREDICTOR_H

#include <iostream>
#include <vector>

typedef unsigned char uchar;
using namespace std;

class predictor {
public:
    predictor(int mode, int width);
    void setMat(uchar* mat);
    short predict();
    int residual();
    void reconstruct(short residual);

private:
    int mode;
    int width;
    int ptr;
    uchar* mat;
};

#endif //PREDICTOR_PREDICTOR_H
