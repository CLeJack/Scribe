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

enum Mode {mono, portamento, poly};

//intentionally offsetting by 1 for log calc later
//precision isn't important hear
const int SINT16 = -1 + std::pow(2,16)/2 ;
const float DBFLOOR = 20 * log10(1/SINT16);
const float DBCEIL = 0;