//
// Created by diego on 16/11/19.
//

#include "predictor.h"
using namespace std;

void predictor::printCircleBuffer(){
    cout << "pointer:\t" << cb_ptr << endl;
    cout << "[ " << circularBuffer[0] << ", " << circularBuffer[1] << ", " << circularBuffer[2] << "]" << endl;
    cout << endl;
}

short predictor::predict()
{   switch(num_inputs) {
        case 0:
            return 0;
        case 1:
            return circularBuffer[0];
        case 2:
            return (short) (2 * ((int) circularBuffer[1]) - ((int) circularBuffer[0]));
        default:
            return (short) (3 * ((int) circularBuffer[cb_ptr]) - 3 * ((int) circularBuffer[(cb_ptr - 1) % 3]) +
                            ((int) circularBuffer[(cb_ptr - 2) % 3]));
    }
}

short predictor::residual(short sample)
{   short pred_val = predict();
    if(!lossy) updateBuffer(sample);
    return (short) (sample - pred_val);
}

void updateBufferQuant(short sample,short quant){
    updateBuffer(sample-predict()+quant);
}

void predictor::updateBuffer(short sample) {
    cb_ptr = (short) ((cb_ptr + 1) % 3);
    circularBuffer[cb_ptr] = sample;
    if(num_inputs < 3) num_inputs++;
}

short predictor::reconstruct(short residual)
{   auto rec_val = (short) (predict() + residual);
    updateBuffer(rec_val);
    return rec_val;
}
