//
// Created by diego on 28/12/19.
//

#ifndef DCT_RLE_H
#define DCT_RLE_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <vector>
#include <ctime>

#include <cassert>
#include <cmath>

using namespace std;

class rle {
public:
    rle(vector<int> vect);
    rle(vector<tuple<int, int>> vect);

    vector<int> rm_fin_zeros();
    vector<int> get_vector();
    vector<tuple<int, int>> get_vector_rle();
    vector<int> reverse_rle_zeros(int num_zeros);

    vector<tuple<int, int>> get_rle();
    vector<tuple<int, int>> get_variant_rle();

    vector<int> load_rle();
    vector<int> load_variant_rle(int n);


    int get_num_zeros();

private:

    vector<int> it_zero_rm(vector<int> vec);
    vector<int> rec_zero_rm(vector<int> vec);
    vector<int> vect;
    vector<tuple<int, int>> vect_rle;

    int get_vector_size();
    int get_variant_vector_size();

    int num_zeros   = -1;
};


#endif //DCT_RLE_H
