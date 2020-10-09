#pragma once

#include "Head.h"
#include "Waveforms.h"
#include "DCT.h"
#include "Stats.h"

void setSineMatrix(fmatrix& sineMat, const cmatrix& matrix, const fvec& timeVector, 
    const fvec& freqs, void (*normFunc) (fvec&));

void setOctErrMatrix1(fmatrix& errMat, const cmatrix& matrix, const fvec& timeVector, 
    const fvec& freqs, void (*normFunc) (fvec&), int octSize = 12);

fvec weightErr(fvec& weights, const fmatrix& matrix, 
int rowi, int rowf, int indi, int indf, float (*statFunc) (const fvec&));

fvec pctErr(fvec& weights, const fmatrix& matrix, 
int rowi, int rowf, int indi, int indf, float (*statFunc) (const fvec&));

fvec normalizeErr(fvec & errs, const fmatrix& matrix);