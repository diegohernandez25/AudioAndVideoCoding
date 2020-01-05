//
// Created by diego on 26/12/19.
//

#ifndef DCT_ZIGZAG_H
#define DCT_ZIGZAG_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <vector>

#include <assert.h>
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace std;
//using namespace cv;

class zigzag {
public:
    zigzag(cv::Mat block);
    zigzag(vector<short> vect, int rows, int cols);

    vector<short> load_zigzag();
    cv::Mat inverse_zigzag();

    int get_rows();
    int get_cols();
    cv::Mat get_mat();
    vector<short> get_vect();


private:
    cv::Mat mat;
    vector<short> vect;
    int rows, cols;

    vector<short> load_zigzag_n_m();
    vector<short> load_zigzag_m_n();
    vector<short> load_zigzag_sqr();

    cv::Mat inverse_zigzag_sqr();
    cv::Mat inverse_zigzag_n_m();
    cv::Mat inverse_zigzag_m_n();
};

#endif //DCT_ZIGZAG_H
