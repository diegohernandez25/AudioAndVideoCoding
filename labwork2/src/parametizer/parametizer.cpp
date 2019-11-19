//
// Created by diego on 19/11/19.
//

#include "parametizer.h"
#include <cassert>

parametizer::parametizer(const string& path_file, uint shift_wind_size, uint shift_wind_size_y,
                         uint shift_wind_skip, uint shift_wind_skip_y, uint m, uint m_y){

    file = path_file;
    shiftWindowSize = shift_wind_size;
    y_shiftWindowSize = shift_wind_size_y;

    shiftWindowSkip = shift_wind_skip;
    y_shiftWindowSkip = shift_wind_skip_y;

    parameters.predOrder=3;

    parameters.totalBits=-1; //Gives you the max value

    parameters.windowSize=0;
    parameters.windowSkip=0;
    parameters.m=m;

    parameters.y_windowSize=50;
    parameters.y_windowSkip=10;
    parameters.y_m = m_y;

}


void parametizer::startLosslessEncoder(){

    uint nbits;
    online_lossless enc_ol(file, (string &) "test_res.wav", false);
    enc_ol.set_pred_order(parameters.predOrder);

    enc_ol.set_y_window_size(parameters.y_windowSize);
    enc_ol.set_y_m_calc_int(parameters.y_windowSkip);
    enc_ol.set_y_initial_m(parameters.y_m);

    enc_ol.set_initial_m(parameters.m);

    uint N = (int) ((maxWindowSize - minWindowSize)/shiftWindowSize);
    //cout << "N:\t" << N << endl;
    for(uint i=minWindowSize, k=0;i<maxWindowSize;i+=shiftWindowSize, k++){
        cout << "\r\e[K(1/3) "<< (int)((100*k/N))<<"%"<<flush;
        enc_ol.set_window_size(i);

        for(uint j=minWindowSkip; j<maxWindowSkip && j<i;j+=shiftWindowSkip){
            enc_ol.set_m_calc_int(j);

            if((nbits=enc_ol.encode()) < parameters.totalBits){
                parameters.totalBits=nbits;
                parameters.windowSize=i;
                parameters.windowSkip=j;
                //cout<<nbits<<","<<i<<","<<j<<endl;

            }
        }
    }

    enc_ol.set_window_size(parameters.windowSize);
    enc_ol.set_m_calc_int(parameters.windowSkip);

    N = (uint)(((uint)y_maxWindowSize - y_minWindowSize)/y_shiftWindowSize);

    for(uint i=y_minWindowSize, k=0;i<y_maxWindowSize;i+=y_shiftWindowSize,k++){
        cout << "\r\e[K(2/3) "<< to_string((int)((100*k/N)))<<"%"<<flush;
        enc_ol.set_y_window_size(i);

        for(uint j=y_minWindowSkip; j<y_maxWindowSkip && j<i;j+=y_shiftWindowSkip){
            enc_ol.set_y_m_calc_int(j);

            if((nbits=enc_ol.encode()) < parameters.totalBits){
                parameters.totalBits=nbits;
                parameters.y_windowSize=i;
                parameters.y_windowSkip=j;
            }
        }
    }

    enc_ol.set_y_window_size(parameters.y_windowSize);
    enc_ol.set_y_m_calc_int(parameters.y_windowSkip);

    cout << "\r\e[K(3/3) Finishing..."<<flush;
    for(uint i=parameters.predOrder -1; i>0; i--){
        enc_ol.set_pred_order(i);
        if((nbits=enc_ol.encode()) < parameters.totalBits){
            parameters.totalBits=nbits;
            parameters.predOrder=i;
        }
    }
}

void parametizer::printParameters(){

    cout << endl;

    cout<<"predOrder:\t\t"<< parameters.predOrder << endl;
    cout<<"windowSize:\t\t"<< parameters.windowSize << endl;
    cout<<"windowSkip:\t\t"<< parameters.windowSkip<< endl;
    cout<<"m:\t\t"<< parameters.m << endl;
    cout<<"y_windowSize:\t\t"<< parameters.y_windowSize<< endl;
    cout<<"y_windowSkip:\t\t"<< parameters.y_windowSkip<< endl;
    cout<<"y_m:\t\t"<< parameters.y_m<< endl;
    cout<<"totalBits:\t\t"<< parameters.totalBits<< endl;

    cout << endl;

}
void parametizer::setFile(string path){file=path;}

param parametizer::getParameters(){return parameters;}

void parametizer::setPredOrder(uint order){parameters.predOrder = order;}

void parametizer::setRangeWindowSize(uint min, uint max){
    assert(min<=max);

    minWindowSize=min;
    maxWindowSize=max;
}

void parametizer::setShiftWindowSize(uint shift){shiftWindowSize=shift;}

void parametizer::setRangeWindowSkip(uint min, uint max){
    assert(min<=max);

    minWindowSkip=min;
    maxWindowSkip=max;
}

void parametizer::setShiftWindowSkip(uint shift){shiftWindowSkip=shift;}

void parametizer::setInitialM(uint m){parameters.m=m;}

void parametizer::setRangeWindowSizeY(uint min, uint max){
    assert(min<=max);

    y_minWindowSize=min;
    y_maxWindowSize=max;
}

void parametizer::setShiftWindowSizeY(uint shift){y_shiftWindowSize=shift;}

void parametizer::setRangeWindowSkipY(uint min, uint max){
    assert(min<=max);

    y_minWindowSkip=min;
    y_maxWindowSkip=max;
}

void parametizer::setShiftWindowSkipY(uint shift){y_shiftWindowSkip=shift;}

void parametizer::setInitialMY(uint m){parameters.y_m=m;}

uint parametizer::getInitialM(){ return parameters.m;}

uint parametizer::getWindowSize(){ return parameters.windowSize;}

uint parametizer::getWindowSkip(){ return parameters.windowSkip;}

uint parametizer::getInitialMY(){ return parameters.y_m;}

uint parametizer::getWindowSizeY(){ return parameters.y_windowSize;}

uint parametizer::getWindowSkipY(){ return parameters.y_windowSkip;}

uint parametizer::getPredOrder(){ return parameters.predOrder;}

parametizer parametizer::presets(uint aggressiveness,string& infile){
    if(aggressiveness==1) { //Moderate agressive
        parametizer pm(infile,200,200,100,100,10,10);
        pm.setRangeWindowSize(10,1500);
        pm.setRangeWindowSizeY(10,1500);
        pm.setRangeWindowSkip(5, 600);
        pm.setRangeWindowSkipY(5, 600);
        pm.startLosslessEncoder();
        return pm;

    }
    else if(aggressiveness==2) { //Agressive
        parametizer pm(infile,200,200,100,100,10,10);
        pm.setRangeWindowSize(10,2000);
        pm.setRangeWindowSizeY(10,2000);
        pm.setRangeWindowSkip(5, 1000);
        pm.setRangeWindowSkipY(5, 1000);
        pm.startLosslessEncoder();
        return pm;

    }
    else if(aggressiveness==3) { //Pretty agressive
        parametizer pm(infile,100,100,100,100,10,10);
        pm.setRangeWindowSize(10,2000);
        pm.setRangeWindowSizeY(10,2000);
        pm.setRangeWindowSkip(5, 1000);
        pm.setRangeWindowSkipY(5, 1000);
        pm.startLosslessEncoder();
        return pm;

    }
    else if(aggressiveness==4) { //"Stop hurting me" agressive
        parametizer pm(infile,100,100,100,100,10,10);
        pm.setRangeWindowSize(1000,4000);
        pm.setRangeWindowSizeY(1000,4000);
        pm.setRangeWindowSkip(5, 1000);
        pm.setRangeWindowSkipY(5, 1000);
        pm.startLosslessEncoder();
        return pm;

    }
    else { //Passive
        parametizer pm(infile, 1500, 1500, 150, 150, 10, 10);
        pm.setRangeWindowSize(10, 10000);
        pm.setRangeWindowSizeY(10, 10000);
        pm.setRangeWindowSkip(10, 1000);
        pm.setRangeWindowSkipY(10, 1000);
        pm.startLosslessEncoder();
        return pm;

    }

}
