#pragma once
#include "Head.h"

// Tuning calculations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline float getEqualTemperamentFrequency(float refFreq, float semitone, int exponent)
{
    return refFreq * pow(semitone, exponent);
}

inline int getEqualTemperamentExponent(float freq, float refFreq, float semitone)
{
    float semitoneToExp = freq / refFreq;
    float exponent = std::log(semitoneToExp) / std::log(semitone); 
    return int(0.5 + exponent);
}

inline int getIndex(float freq, float refFreq, float semitone, int minExponent)
{
    int exponent = getEqualTemperamentExponent(freq, refFreq, semitone);
    exponent = std::max(minExponent, exponent);
    
    //make minExponent = 0 index in freq array
    exponent = exponent - minExponent; 
    return exponent;
}

//pitch class. I.e. C, D, G, G# etc. indices [0,11] for 12Tone Equal Temperament
inline int getClassIndex(float freq, float refFreq, float semitone, int minExponent, const strvec& noteNames)
{
    int index =  getIndex(freq, refFreq, semitone, minExponent );
    index = index % noteNames.size();
    return index;
}

inline std::string getClassName(float freq, float refFreq, float semitone, int minExponent, const strvec& noteNames)
{
    return noteNames[getClassIndex(freq, refFreq, semitone, minExponent, noteNames)];
}

inline int getOctave(float freq, float refFreq, float semitone, int minExponent)
{
    int index =  getIndex(freq, refFreq, semitone, minExponent);
    int octave = int(index/12);
    return octave;
}

inline float octIndToFreq(const fvec& frequencies, int octave, int pitch, int noteCount)
{
    int freqInd = octave*noteCount + pitch;
    return frequencies[freqInd];
}

inline fvec getOctFrequencies(const fvec& refFreqs, int octSize)
{
    fvec output(octSize, 0);

    for(int i = 0; i < output.size(); i++)
    {
        output[i] = refFreqs[i * octSize];
    }
    return output;
}

// fill frequency arrays ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline void setETFrequencies(fvec& freqs, float refFreq, float semitone, int lowExp, int highExp)
{
    int i = 0;
    for(int exp = lowExp; exp < highExp + 1; exp++)
    {
        freqs[i] = getEqualTemperamentFrequency(refFreq, semitone, exp);
        i++;
    }

}

inline void expandFrequencies(fvec& freqs, fvec& exFreqs)
{
    //linear interpolation between frequencies

    for(int i = 0; i<freqs.size() -1; i++)
    {
        int ind = 3*i;
        exFreqs[ind] = freqs[i];

        float interval = (freqs[i+1] - freqs[i] )/3.0;
        exFreqs[ind + 1] = exFreqs[ind] + interval;
        exFreqs[ind + 2] = exFreqs[ind] + 2*interval;
    }

    
    exFreqs[exFreqs.size()-1] = freqs[freqs.size() -1];

}

