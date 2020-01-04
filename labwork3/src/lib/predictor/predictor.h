#ifndef PREDICTOR_PREDICTOR_H
#define PREDICTOR_PREDICTOR_H

#include <iostream>
#include <vector>
#include <tuple>

#include <opencv2/core.hpp>

using namespace std;

class predictor {
public:
    predictor(){};
    predictor(ushort width, ushort height);
    void newFrame(cv::Mat* mat);
    short predict(ushort x, ushort y, uint8_t mode);
    short calcResidual(ushort x, ushort y, uint8_t mode);
    void calcBlockResiduals(ushort x, ushort y, uint8_t mode, cv::Mat* resMat);
    // mode, score
    tuple<uint8_t, uint> calcBestResiduals(ushort x, ushort y, cv::Mat* resMat);
    void reconstruct(ushort x, ushort y, uint8_t mode, short residual);
    void reconstructBlock(ushort x, ushort y, uint8_t mode, cv::Mat* resMat);

private:
    ushort width, height;
    cv::Mat* origMat;
};

#endif //PREDICTOR_PREDICTOR_H
