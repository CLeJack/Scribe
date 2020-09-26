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
    int loOct = 0;
    int octStr = 0;
    int loNote = 0;

    int noise = 0;
    int release = 0;

    float weightThreshold = 0;

    float trigStart = 0;
    float retrigStart = 0;
    float retrigStop = 0;

    int smooth = 0;

    int octave = 0;
    int semitone = 0;

    int velDbMin = 0;
    int velDbMax = 0;
    int velMin = 0;
    int velMax = 0;

    int channelIn = 0;
};

struct Calculations 
{
    //holds data calculated within the audio block or a simulation of an audio block


    //combining all of this data in structs adds a lot of abstraction
    //I'm keep function calls explicit so the process is more transparent within the audioblock
    void updateRangeInfo(const AudioParams& params, int signalSize);
    void updateSignalInfo(const fvec& weights, const fvec& ratios, const fvec& signal, const AudioParams& params);
    void updateSignalInfo(const fvec& weights, const fvec& signal, const AudioParams& params);
    void updateMidiNum(const Storage& storage, const Properties& props, const AudioParams params);
    
    //range info
    int loNote = 0;
    int hiNote = 0;
    int signalStart = 0;

    // signal indo
    int f0ind = 0;
    int f0oct = 0;
    int f0pitch = 0;
    float f0ratio = 0;
    
    int noteInd = 0;
    int noteOct = 0;
    int notePitch = 0;
    float noteRatio = 0;

    float weight = 0;

    float ampFull = 0;
    float ampHalf1 = 0;
    float ampHalf2 = 0;
    
    float trigger = 0;
    float retrigger = 0;
    float ampdB = 0;

    // midinum
    int midiNum = 0;
};


fvec getPeaks(const fvec& signal);
void alignWithPeaks(fvec& arr, const fvec& peaks);

void clearBelowInd(fvec& arr, int ind);
void clearAboveInd(fvec& arr, int ind);

fvec weightRatio(const fvec& arr, int octSize);

fvec getRelativeOctaveWeight(const fvec& weights, int pitchIndex, int loNote, int octSize = 12);

int correctOctave(const fvec& weights, int pitchIndex, int loNote, int octSize = 12);

MidiParams getMidiParams(const Calculations& calcs, const AudioParams& params);