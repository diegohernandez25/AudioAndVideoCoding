//
// Created by diego on 29/12/19.
//

#ifndef DCT_DCT_H
#define DCT_DCT_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <vector>
#include <ctime>
#include <tuple>

#include <cassert>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "zigzag/zigzag.h"
#include "rle/rle.h"

#define BLOCK_SIZE 8

using namespace std;

class dct{
public:
    dct(int height, int width);
    dct(cv::Mat image);

    int get_height_blk();
    int get_width_blk();
    int get_total_blk();

    int get_height_padded();
    int get_width_padded();

    int get_rstr_scnr_blk_ptr();

    cv::Mat get_image();
    cv::Mat get_padded_image();
    cv::Mat get_rcvrd_image();


    vector<tuple<short,short>> dct_quant_rle_blck(cv::Mat blck, bool mask=false);
    cv::Mat reverse_dct_quant_rle_blck(vector<tuple<short, short>> rle_vctr, bool mask=false);

    cv::Mat reverse_dct_quant_rle_frame(vector<vector<tuple<short,short>>> rle_vctrs, bool mask=false);
    void dct_quant_rle_frame(bool mask=false);
    bool upload_coded_blck(vector<tuple<short, short>> rle_vct, bool mask=false);

    vector<vector<tuple<short,short>>> get_vect();

private:
    int height_blk;
    int width_blk;
    int total_blk;

    int height_padded;
    int width_padded;

    int rstr_scnr_blk_ptr;

    short quant_array[64]={   16, 11, 10, 16, 24, 40, 51, 61,
                            12, 12, 14, 19, 26, 58, 60, 55,
                            14, 13, 16, 24, 40, 57, 69, 56,
                            14, 17, 22, 29, 51, 87, 80, 62,
                            18, 22, 37, 56, 68, 109, 103, 77,
                            24, 35, 55, 64, 81, 104, 113, 92,
                            49, 64, 78, 87, 103, 121, 129, 101,
                            72, 92, 95, 98, 112, 100, 103, 99};

    int quant_array_mask[64] = {1, 1, 1, 1, 1, 1, 1, 1,
                                1, 1, 1, 1, 1, 1, 1, 0,
                                1, 1, 1, 1, 1, 1, 0, 0,
                                1, 1, 1, 1, 1, 0, 0, 0,
                                1, 1, 1, 1, 0, 0, 0, 0,
                                1, 1, 1, 0, 0, 0, 0, 0,
                                1, 1, 0, 0, 0, 0, 0, 0,
                                1, 0, 0, 0, 0, 0, 0, 0,};

    cv::Mat quant_mat = cv::Mat(8, 8, CV_16S, quant_array);
    cv::Mat quant_mat_mask = cv::Mat(8, 8, CV_16S, quant_array_mask);

    cv::Mat image;
    cv::Mat padded_image;
    cv::Mat rcvrd_image;

    vector<vector<tuple<short,short>>> vect;
};

#endif //DCT_DCT_H
