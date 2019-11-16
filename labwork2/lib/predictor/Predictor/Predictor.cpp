//
// Created by diego on 16/11/19.
//

#include "Predictor.h"
using namespace std;

void Predictor::printCircleBuffer(){
    cout << "pointer:\t" << cb_ptr << endl;
    cout << "[ " << circularBuffer[0] << ", " << circularBuffer[1] << ", " << circularBuffer[2] << "]" << endl;
    cout << endl;
}

short Predictor::predict()
{   switch(num_inputs) {
        case 0:
            return 0;
        case 1:
            return circularBuffer[0];
        case 2:
            return (short) (2 * circularBuffer[1] - circularBuffer[0]);
        default:
            return (short) (3 * circularBuffer[cb_ptr] - 3 * circularBuffer[(cb_ptr - 1) % 3] +
                            circularBuffer[(cb_ptr - 2) % 3]);
    }
}

short Predictor::residual(short sample)
{   short pred_val = predict();
    updateBuffer(sample);
    return (short) (sample - pred_val);
}

void Predictor::updateBuffer(short sample) {
    cb_ptr = (short) ((cb_ptr + 1) % 3);
    circularBuffer[cb_ptr] = sample;
    if(num_inputs < 3) num_inputs++;
}

short Predictor::reconstruct(short residual)
{   auto rec_val = (short) (predict() + residual);
    updateBuffer(rec_val);
    return rec_val;
}
