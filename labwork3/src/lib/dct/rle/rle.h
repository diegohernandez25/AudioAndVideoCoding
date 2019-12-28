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

    vector<int> rm_fin_zeros();
    vector<int> get_vector();
    int get_num_zeros();

private:

    vector<int> it_zero_rm(vector<int> vec);
    vector<int> rec_zero_rm(vector<int> vec);
    vector<int> vect;

    int num_zeros   = -1;
};


#endif //DCT_RLE_H
