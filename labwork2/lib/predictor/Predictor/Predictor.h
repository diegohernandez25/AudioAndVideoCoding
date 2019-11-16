//
// Created by diego on 16/11/19.
//

#ifndef PREDICTOR_PREDICTOR_H
#define PREDICTOR_PREDICTOR_H
#include <iostream>

using namespace std;

class Predictor {
public:
    short predict();
    short residual(short sample);
    void printCircleBuffer();
    short reconstruct(short );

    Predictor()
    {   num_inputs = 0;
        cb_ptr = 2;
    }

private:
    void  updateBuffer(short sample);
    int num_inputs;
    short cb_ptr;
    short circularBuffer[3] = {0,0,0};

};


#endif //PREDICTOR_PREDICTOR_H
