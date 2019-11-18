//
// Created by diego on 18/11/19.
//

#include "quant.h"


short quant::midrise(uint16_t nbits, short value)
{
    return (value >> nbits);
}

short quant::midtread(uint16_t nbits, short value)
{
    short tmp = value + (1 << nbits) / 2;
    if(value > tmp)
        tmp = (1 << 15) - 1;
    return midrise(nbits,tmp);
}