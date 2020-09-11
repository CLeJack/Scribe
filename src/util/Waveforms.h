#pragma once
#include "Head.h"


fvec getTimeVector(float time, float srate);

fvec getTimeVector(int samples, float srate);

fvec sinusoid(const fvec& t, float frequency, float phaseDeg, float amplitude);

fvec sinusoid(float time, float srate, float frequency, float phaseDeg, float amplitude);

fvec sinusoid(int samples, float srate, float frequency, float phaseDeg, float amplitude);

cvec complex_sinusoid(const fvec& t, float frequency, float phaseDeg, float amplitude);

cvec complex_sinusoid(float time, float srate, float frequency, float phaseDeg, float amplitude);

cvec complex_sinusoid(int samples, float srate, float frequency, float phaseDeg, float amplitude);

//fwhm = full width at half maximum
float gaussian(float x, float amplitude, float fwhm);

fvec gaussianComb(const fvec& t, float freq, float phaseDeg, float amplitude, float fwhm);

fmatrix gaussianOctaves(const fvec& frequencies, int octSize);
