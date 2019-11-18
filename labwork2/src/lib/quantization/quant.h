//
// Created by diego on 18/11/19.
//

#ifndef QUANTIZATION_QUANT_H
#define QUANTIZATION_QUANT_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <string>
#include <cmath>
#include <fstream>

using namespace std;

class quant {
public:
    short midrise(uint16_t nbits, short value);
    short midtread(uint16_t nbits, short value);

private:

};


#endif //QUANTIZATION_QUANT_H
