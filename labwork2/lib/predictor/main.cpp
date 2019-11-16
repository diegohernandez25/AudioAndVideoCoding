#include <iostream>
#include <cstring>
#include <string>

#include "Predictor/Predictor.h"

using namespace std;

int main(){
    string number;
    int sample, resi_val, rec_val;

    Predictor pred = Predictor();
    Predictor rec = Predictor();
    for(;;)
    {   cout << "n: ";
        getline(cin, number);
        if(!number.compare("e"))
            break;

        sample = (short) stoi(number);
        short resi_val = pred.residual(sample);
        short rec_val = rec.reconstruct(resi_val);

        cout << "residual:\t" << resi_val << endl;
        pred.printCircleBuffer();
        cout << "reconstruct:\t" <<rec_val<<endl;
        rec.printCircleBuffer();
    }
}