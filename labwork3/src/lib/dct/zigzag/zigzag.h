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
    zigzag(vector<int> vect);


    vector<int> load_zigzag();
    cv::Mat inverse_zigzag();
    cv::Mat get_mat();
    vector<int> get_vect();


private:
    cv::Mat mat;
    vector<int> vect;
};


#endif //DCT_ZIGZAG_H