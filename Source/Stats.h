#pragma once
#include "Head.h"

float mean(const fvec& arr, int start, int end);
float mean(const fvec& arr, int start);
float mean(const fvec& arr);

float positiveMean(const fvec& arr, int start, int end);

float rms(const fvec& arr, int start, int end);
float rms(const fvec& arr, int start);
float rms(const fvec& arr);

float maxValue(const fvec& arr, int start, int end);
float maxValue(const fvec& arr, int start);
float maxValue(const fvec& arr);

float absMaxValue(const fvec& arr, int start, int end);
float absMaxValue(const fvec& arr, int start);
float absMaxValue(const fvec& arr);

int maxArg(const fvec& arr, int start, int end);
int maxArg(const fvec& arr, int start);
int maxArg(const fvec& arr);

float minValue(const fvec& arr, int start, int end);
float minValue(const fvec& arr, int start);
float minValue(const fvec& arr);

int minArg(const fvec& arr, int start, int end);
int minArg(const fvec& arr, int start);
int minArg(const fvec& arr);


fvec absMaxNormalize(const fvec& arr, int zero);
void absMaxNormalize(fvec& arr);

fvec sumNormalize(const fvec& arr, int zero);
void sumNormalize(fvec& arr);

float sum(const fvec& arr);

fvec pctDiff(const fvec& arr);
fvec diff(const fvec& arr);


//simple linear regression
fvec linreg(const fvec& x, const fvec& y);
fvec line(const fvec& regOutput, int len);

//center of mass

float CoMY(const fvec& arr);
fvec CoMY2(const fvec& arr);
fvec CoMY3(const fvec& arr);

float CoMX(const fvec& arr);
fvec CoMX2(const fvec& arr);



fvec hadamardX(const fvec& arr1, const fvec& arr2);

fvec add(const fvec& arr1, const fvec& arr2);

//simple moving average without storage
float SMA(float hist, float val, float size);