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
    int residual(short sample);
    short reconstruct(int residual);
    void updateBufferQuant(short quant);
    void printCircleBuffer();

    predictor(bool lossy,int order){
		this->order=order;
		this->lossy = lossy;
		num_inputs = 0;
        cb_ptr = 2;
    }

private:
    void  updateBuffer(short sample);
    int num_inputs;
    short cb_ptr;
    short circularBuffer[3] = {0,0,0};
    bool lossy;
	int order;
};


#endif //PREDICTOR_PREDICTOR_H
