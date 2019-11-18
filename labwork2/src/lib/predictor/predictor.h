//
// Created by diego on 16/11/19.
//

#ifndef PREDICTOR_PREDICTOR_H
#define PREDICTOR_PREDICTOR_H
#include <iostream>

using namespace std;

class predictor {
public:
    short predict();
    short residual(short sample);
    short reconstruct(short residual);
    void updateBufferQuant(short sample,short quant);
    void printCircleBuffer();

    predictor(bool lossy)
    {   this->lossy = lossy;
	num_inputs = 0;
        cb_ptr = 2;
    }

private:
    void  updateBuffer(short sample);
    int num_inputs;
    short cb_ptr;
    short circularBuffer[3] = {0,0,0};
    bool lossy;
};


#endif //PREDICTOR_PREDICTOR_H
