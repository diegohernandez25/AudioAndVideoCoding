//
// Created by diego on 19/11/19.
//

#ifndef PARAMETIZER_PARAMETIZER_H
#define PARAMETIZER_PARAMETIZER_H


#include <sys/param.h>
#include <string>
#include <iostream>

#include "../lossless/online_lossless.h"
#include "../lossless/offline_lossless.h"
#include "../lossy/online_lossy.h"

using namespace std;

typedef struct PARAMETERS{
    uint predOrder;

    uint windowSize;
    uint windowSkip;
    uint m;

    uint y_windowSize;
    uint y_windowSkip;
    uint y_m;

    uint totalBits;
}param;

class parametizer {

public:

    parametizer(const string& file, uint shift_wind_size, uint shift_wind_size_y,
                uint shift_wind_skip, uint shift_wind_skip_y, uint m, uint m_y);

    void startLosslessEncoder();

    void setFile(string path);

    void setPredOrder(uint order);

    void setRangeWindowSize(uint min, uint max);
    void setShiftWindowSize(uint shift);

    void setRangeWindowSkip(uint min, uint max);
    void setShiftWindowSkip(uint shift);

    void setInitialM(uint m);

    void setRangeWindowSizeY(uint min, uint max);
    void setShiftWindowSizeY(uint shift);

    void setRangeWindowSkipY(uint min, uint max);
    void setShiftWindowSkipY(uint shift);

    void setInitialMY(uint m);

    void printParameters();

    param getParameters();

    uint getWindowSize();
    uint getWindowSkip();
    uint getWindowSizeY();
    uint getWindowSkipY();
    uint getPredOrder();


private:
    string file;

    param parameters{};

    uint maxWindowSize{};
    uint minWindowSize{};
    uint shiftWindowSize;

    uint maxWindowSkip{};
    uint minWindowSkip{};
    uint shiftWindowSkip;

    uint y_maxWindowSize{};
    uint y_minWindowSize{};
    uint y_shiftWindowSize;

    uint y_maxWindowSkip{};
    uint y_minWindowSkip{};
    uint y_shiftWindowSkip;



};


#endif //PARAMETIZER_PARAMETIZER_H
