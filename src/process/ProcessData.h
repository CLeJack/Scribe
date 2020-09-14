#pragma once

#include "Head.h"
#include "Waveforms.h"
#include "Tuning.h"
#include "CircularBuffer.h"
#include "Stats.h"
#include "DCT.h"
#include "MidiSwitch.h"

struct Properties
{
    Properties();
    Properties(float srate, float blockSize);

    float blockSize = 128;
    float srate = 44100;
    const float histTime = .05;

    const float refFreq = 440; //concert tuning
    const float semitone = std::pow(2, 1.0/12.0); //12 Tone equal temperament
    const int octaveSize = 12;
    
    

    //float lowExp = -57 + 12; // -57 = C0
    int lowExp = -57; // -57 = C0
    int highExp = 50 - 24 + 1; // 50 = B8; aliasing exist in octaves 7 to 8;

    int freqSize = 0;
    int histSamples = 0;
    int transBlocks = 1;

    

    float dsRate = 4000;
    int dsFactor = 0;
    int dsHistSamples = 0;
    int dsBlock = 0;

    //used in the main loop to allow for cleaner downsampling;
    int dsIndex = 0;
    
};



struct Storage
{
    Storage();
    Storage(Properties& props);
    std::unique_ptr<fvec> frequencies;
    std::unique_ptr<fvec> exFrequencies;

    std::unique_ptr<fvec> timeVector;

    std::unique_ptr<cmatrix> matrix;
    std::unique_ptr<cmatrix> exMatrix;

    std::unique_ptr<FloatBuffer> history;
    std::unique_ptr<FloatBuffer> historyDS;

    std::unique_ptr<MidiSwitch> midiSwitch;
    
};


fvec getPeaks(const fvec& signal);
void alignWithPeaks(fvec& arr, const fvec& peaks);

void clearBelowInd(fvec& arr, int ind);
void clearAboveInd(fvec& arr, int ind);

fvec weightRatio(const fvec& arr, int octSize);


