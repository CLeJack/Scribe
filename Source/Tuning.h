#pragma once
#include "Head.h"

// Tuning calculations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float getEqualTemperamentFrequency(float refFreq, float semitone, int exponent);

int getEqualTemperamentExponent(float freq, float refFreq, float semitone);

int getIndex(float freq, float refFreq, float semitone, int minExponent);

//pitch class. I.e. C, D, G, G# etc. indices [0,11] for 12Tone Equal Temperament
int getClassIndex(float freq, float refFreq, float semitone, int minExponent, const strvec& noteNames);

std::string getClassName(float freq, float refFreq, float semitone, int minExponent, const strvec& noteNames);

int getOctave(float freq, float refFreq, float semitone, int minExponent);

float octIndToFreq(const fvec& frequencies, int octave, int pitch, int noteCount);

fvec getOctFrequencies(const fvec& refFreqs, int octSize);

// fill frequency arrays ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setETFrequencies(fvec& freqs, float refFreq, float semitone, int lowExp, int highExp);

void expandFrequencies(fvec& freqs, fvec& exFreqs);

fvec crush(const fvec& exArr, int finalSize, int shift, int factor);


