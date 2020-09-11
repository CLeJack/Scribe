#pragma once
#include "Head.h"

//debugging
void printColumn(fvec& arr, std::string title);

void printColumn(float val, std::string title);

void printRows(const fvec& arr, std::string title);

void printRows(const ivec& arr, std::string title);

void printMatrix(fmatrix& mat, std::string title, int id);

void printMatrix(cmatrix& mat, std::string title, int id);

void printMatrixReal(cmatrix& mat, std::string title, int id);

fvec importCsv(std::string filename, int vecsize);

void printLine(fvec& arr, std::string title);

void printLine(ivec& arr, std::string title);

void printElement(float val);

// non debugging~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
fvec crush(const fvec& exArr, int finalSize, int shift, int factor);

void clearLastOctave(fvec& arr, int octSize);

void clearBelowInd(fvec& arr, int ind);
void clearAboveInd(fvec& arr, int ind);

void clearQuietNotes(fvec& amps, float noiseThresh);

void alignWithPeaks(fvec& arr, const fvec& peaks);



fvec getAmplitudes(const fvec& weights, float amp);

fvec getPitchClassWeights(const fvec& weights, int octSize, int start);

fvec getOctaveWeights(const fvec& weights, int octCount);

void joinIntervals(fvec& arr, int semitones);

fvec hadamardX(const fvec& arr1, const fvec& arr2);

void updateVec(fvec& target, const fvec& newValues);

void fill(fvec& arr, float value);


float normToInt16Range(float val);

float int16ToDb(float val);

float float32ToDb(float val);

fvec weightRatio(const fvec& arr, int octSize);







