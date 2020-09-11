#pragma once

#include <vector>
#include <cmath>
#include <complex>
#include <string> 
#include <memory>
#include <limits>
#include <iostream>
#include <fstream>

typedef std::vector<int> ivec;
typedef std::vector<float> fvec;
typedef std::vector<std::complex<float>> cvec;
typedef std::vector<cvec> cmatrix;
typedef std::vector<fvec> fmatrix;
typedef std::vector<std::string> strvec;

//Scribe int 16
const int SINT16 = -1 + std::pow(2,16)/2 ;
const float DBFLOOR = 20 * log10(1/SINT16);
const float DBCEIL = 0;

inline float normToInt16Range(float val)
{
    return val*SINT16;
}

inline float int16ToDb(float amp)
{
    return 20 * log10( amp/SINT16);
}

inline float dbToInt16(float db)
{
    return std::pow(10, db/20.0f)* SINT16;
}
