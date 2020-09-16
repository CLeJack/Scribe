#include "Tuning.h"

// Tuning calculations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float getEqualTemperamentFrequency(float refFreq, float semitone, int exponent)
{
    return refFreq * pow(semitone, exponent);
}

int getEqualTemperamentExponent(float freq, float refFreq, float semitone)
{
    float semitoneToExp = freq / refFreq;
    float exponent = std::log(semitoneToExp) / std::log(semitone); 
    return int(0.5 + exponent);
}

int getIndex(float freq, float refFreq, float semitone, int minExponent)
{
    int exponent = getEqualTemperamentExponent(freq, refFreq, semitone);
    exponent = std::max(minExponent, exponent);
    
    //make minExponent = 0 index in freq array
    exponent = exponent - minExponent; 
    return exponent;
}

//pitch class. I.e. C, D, G, G# etc. indices [0,11] for 12Tone Equal Temperament
int getClassIndex(float freq, float refFreq, float semitone, int minExponent, const strvec& noteNames)
{
    int index =  getIndex(freq, refFreq, semitone, minExponent );
    index = index % noteNames.size();
    return index;
}

std::string getClassName(float freq, float refFreq, float semitone, int minExponent, const strvec& noteNames)
{
    return noteNames[getClassIndex(freq, refFreq, semitone, minExponent, noteNames)];
}

int getOctave(float freq, float refFreq, float semitone, int minExponent)
{
    int index =  getIndex(freq, refFreq, semitone, minExponent);
    int octave = int(index/12);
    return octave;
}

float octIndToFreq(const fvec& frequencies, int octave, int pitch, int noteCount)
{
    int freqInd = octave*noteCount + pitch;
    return frequencies[freqInd];
}

fvec getOctFrequencies(const fvec& refFreqs, int octSize)
{
    fvec output(octSize, 0);

    for(int i = 0; i < output.size(); i++)
    {
        output[i] = refFreqs[i * octSize];
    }
    return output;
}

// fill frequency arrays ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setETFrequencies(fvec& freqs, float refFreq, float semitone, int lowExp, int highExp)
{
    int i = 0;
    for(int exp = lowExp; exp < highExp + 1; exp++)
    {
        freqs[i] = getEqualTemperamentFrequency(refFreq, semitone, exp);
        i++;
    }

}

void expandFrequencies(fvec& freqs, fvec& exFreqs)
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

fvec crush(const fvec& exArr, int finalSize, int shift, int factor)
{   //default shift = 1, factor = 3
    fvec output(finalSize,0);
    int ind = 0;
    for(int exInd = 0; exInd < exArr.size(); exInd++)
    {
        ind =int((exInd + shift)/factor);

        //choose the best value between all potential values at this index
        output[ind] = std::max(output[ind],exArr[exInd]);
    }

    return output;
}