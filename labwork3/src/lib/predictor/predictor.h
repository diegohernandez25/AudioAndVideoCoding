#ifndef PREDICTOR_PREDICTOR_H
#define PREDICTOR_PREDICTOR_H

#include <iostream>
#include <vector>
#include <tuple>

#include <opencv2/core.hpp>

using namespace std;

class predictor {
public:
    predictor(short width, short height);
    void newFrame(cv::Mat* mat);
    short predict(short x, short y, uint8_t mode);
    short calcResidual(short x, short y, uint8_t mode);
    void calcBlockResiduals(short x, short y, uint8_t mode, cv::Mat* resMat);
    void reconstruct(short x, short y, uint8_t mode, short residual);
    void reconstructBlock(short x, short y, uint8_t mode, cv::Mat* resMat);
    // mode, score
    tuple<uint8_t, uint> calcBestResiduals(short x, short y, cv::Mat* resMat);

private:
    short width, height;
    cv::Mat* origMat;
};

#endif //PREDICTOR_PREDICTOR_H
