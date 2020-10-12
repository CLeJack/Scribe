#pragma once

#include "Head.h"
#include "Waveforms.h"
#include "DCT.h"
#include "Stats.h"

void setSineMatrix(fmatrix& sineMat, const cmatrix& matrix, const fvec& timeVector, 
    const fvec& freqs, void (*normFunc) (fvec&));

void setOctErrMatrix1(fmatrix& errMat, const cmatrix& matrix, const fvec& timeVector, 
    const fvec& freqs, void (*normFunc) (fvec&), int octSize = 12);

fmatrix freqCertaintyMatrix(const fvec& maxWeights, const fmatrix& maxMatrix, 
int rowi, int rowf, int indi, int indf);

fvec freqCertaintyVector(const fvec& sumWeights, const fmatrix& freqMatrix,
int rowi, int rowf, int indi, int indf);