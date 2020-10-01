#pragma once

#include "Head.h"

// Discrete Customized Transform

void dct(fvec& weights, const cmatrix& matrix, const fvec& signal, 
                int rowi, int rowf, int indi, int indf);

fvec dct(const cmatrix& matrix, const fvec& signal, 
                int rowi, int rowf, int indi, int indf);

fvec dct(const cmatrix& matrix, const fvec& signal);

fvec dctSignalAvg(const fvec& weights, float signalSize);

void setComplexMatrix(cmatrix& matrix, const fvec& freqs, const fvec& timeVector);