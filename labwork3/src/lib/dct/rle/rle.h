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
    rle(vector<short> vect);
    rle(vector<tuple<short, short>> vect);

    vector<short> rm_fin_zeros();
    vector<short> get_vector();
    vector<tuple<short, short>> get_vector_rle();
    vector<short> reverse_rle_zeros(int num_zeros);

    vector<tuple<short, short>> get_rle();
    vector<tuple<short, short>> get_variant_rle();

    vector<short> load_rle();
    vector<short> load_variant_rle(int n);

    int get_num_zeros();

private:

    vector<short> it_zero_rm(vector<short> vec);
    vector<short> rec_zero_rm(vector<short> vec);
    vector<short> vect;
    vector<tuple<short, short>> vect_rle;

    int get_vector_size();
    int get_variant_vector_size();

    int num_zeros   = -1;
};


#endif //DCT_RLE_H
