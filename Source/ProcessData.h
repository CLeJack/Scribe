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
    const float histTime = .05f;

    const float refFreq = 440; //concert tuning
    const float semitone = float(std::pow(2, 1.0/12.0)); //12 Tone equal temperament
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

struct AudioParams 
{
    //holds data provided by the plugin and used as reference for
    //frequency or midi calculations
    int loOct;
    int octStr;

    int noise;
    int release;

    float weight;

    float retrigStart;
    float retrigStop;

    int smooth;

    int octave;
    int semitone;

    int velDbMin;
    int velDbMax;
    int velMin;
    int velMax;

    int channelIn;
};

struct Calculations 
{
    //holds data calculated within the audio block or a simulation of an audio block


    //combining all of this data in structs adds a lot of abstraction
    //I'm keep function calls explicit so the process is more transparent within the audioblock
    void updateRangeInfo(const AudioParams& params, int signalSize);
    void updateSignalInfo(const fvec& weights, const fvec& ratios, const fvec& signal, const AudioParams& params);
    void updateMidiNum(const Storage& storage, const Properties& props, const AudioParams params);
    
    int loNote;
    int hiNote;
    int signalStart;

    int f0ind;
    int f0oct;
    int f0pitch;
    float f0ratio;
    
    int noteInd;
    int noteOct;
    int notePitch;
    float noteRatio;

    float ampFull;
    float ampHalf;
    float trigger;
    float retrigger;
    float ampdB;

    int midiNum;
};


fvec getPeaks(const fvec& signal);
void alignWithPeaks(fvec& arr, const fvec& peaks);

void clearBelowInd(fvec& arr, int ind);
void clearAboveInd(fvec& arr, int ind);

fvec weightRatio(const fvec& arr, int octSize);



