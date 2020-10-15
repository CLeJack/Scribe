#pragma once
#include "Head.h"
#include "Waveforms.h"
#include "Tuning.h"
#include "CircularBuffer.h"
#include "Stats.h"
#include "DCT.h"
#include "Likelihood.h"
#include "ProcessData.h"
#include "MidiSwitch.h"


/*
* 
0. Shared Structs
1. Scribe Model
2. AudioParam Model
3. Calculations Model

*/

// 0.Shared Structs~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct Range 
{
    int lowNote  = 28;
    int highNote = lowNote + 48;
};

struct Threshold
{

    float release = -60;
    float noise   = -50;

    float certainty = 0.7f;

    float trigger = 0.75f;
    float retrig = 0.95f;
    float retrigStop  = 1.0f;
};

struct Scale
{
    float noise  = -5;
    float weight = 0.5f;
};

struct SmoothTime //corresponds with Calculation Delay 
{
    //milliseconds
    float midi     = 25;
    float dBShort  = 5; //shorter smoothing time
    float dBLong   = 25; //longer smoothing time
};

struct Shift
{
    int octave   = 0;
    int semitone = 0;
};

struct Velocity
{
    int min = 50;
    int max = 127;

    float maxdB = -20;
};

//calculation specific

enum class TriggerState { trigger, retriggering, stable };

struct Amp
{
    float val   = 0;
    float half1 = 0;
    float half2 = 0;
    float dB    = 0;
    float factor = 0;
};

struct Delay
{
    float dBShort = -90;
    float dBLong  = -90;
};

struct Blocks
{
    float midi = 1;
    float dBShort  = 1;
    float dBLong   = 1;
};

struct Note
{
    float index   = 0;
    float octave  = 0;
    float pitch   = 0;
    float ratio   = 0;
    float history = 0;
};



//scribe specific
struct Tuning
{
    const float refFreq = 440; //(Hz) concert tuning
    const float semitone = std::pow(2, 1.0 / 12.0); //12 Tone equal temperament
    const int octaveSize = 12;

    const int lowExp = -57; // -57 = C0
    const int highExp = 50 - 24; // 50 = B8; aliasing exist in octaves 7 to 8;
};

struct Audio
{
    static float historyTime() { return 0.05f; }

    struct DownSample
    {
        const float srate = 4000;

        const int samples = srate * Audio::historyTime(); //size of history not blocksize

        //for use with DCT
        //full signal start calculates octave 0 which isn't needed
        //half signal starts calculations on octave 1 which should be bass guitar range
        const int signalStart = samples / 2;

        int factor = 0;
        float blockSize = 0;
    };

    int samples = 0; //size of history not blocksize

    float srate = 44100;
    int blockSize = 128;
    float mindB = -90;
    DownSample ds;
};

//~~~~~~~~~~~~~~~~~~~~~~

// 1. Scribe program data model~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Scribe {

    void initialize(float srate, float blockSize);//set all required non-const variables here
    bool detectsPropertyChange(float srate, float blockSize);
    void updateWeights(int lowNote, int highNote, int midiBlocks, const Amp& amp, const Threshold& thresh);
    void updateMidiInfo(
        const Threshold& thresh, const Amp& amp, const Velocity& vel, 
        const Range& range, const Shift& shift);

    void turnOnMidi(int i);
    void turnOffMidi(int i);

    bool isInitialized = false; //don't run PluginProcessor ready state loop without this set to true

    Tuning tuning;
    Audio audio;

    fvec frequencies = fvec(1 + tuning.highExp - tuning.lowExp, 0);
    ivec midiNumbers = ivec(frequencies.size(), 0);

    fvec weights = fvec(frequencies.size(), 0);
    fvec maxWeights = fvec(frequencies.size(), 0);
    fvec sumWeights = fvec(frequencies.size(), 0);
    fvec certainty = fvec(frequencies.size(), 0);
    fvec weightHistory = fvec(frequencies.size(), 0);
    fvec peaks = fvec(frequencies.size(), 0);
    fvec notedB = fvec(frequencies.size(), -90);
    ivec finalNote = ivec(frequencies.size(), 0);
    
    std::vector<bool> needsTrigger = std::vector<bool>(frequencies.size(), false);
    std::vector<bool> onNotes = std::vector<bool>(frequencies.size(), false);
    std::vector<bool> needsRelease = std::vector<bool>(frequencies.size(), false);
    
    Note fundamental;

    fvec timeVector = fvec(audio.ds.samples, 0);

    cmatrix matrix = cmatrix(frequencies.size(), cvec(audio.ds.samples, std::complex<float>(0, 0)));

    //sum normalization and max normalization
    fmatrix maxOctMatrix = fmatrix(frequencies.size(), fvec(audio.ds.samples,0));
    fmatrix maxSineMatrix = fmatrix(frequencies.size(), fvec(audio.ds.samples,0));


    std::unique_ptr<FloatBuffer> history;

    fvec historyDS = fvec(audio.ds.samples, 0.0001f);

};



// 2. AudioParam data model~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~
struct AudioParams
{
    Range range;
    Threshold threshold;
    Scale scale;
    SmoothTime smoothTime;
    Shift shift;
    Velocity velocity;
};

// 3. Calculations Data Model~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Calculations
{
    //these functions should be called in order
    void updateRange  (const Scribe& scribe, const AudioParams& params);
    void updateSignal (const Scribe& scribe, const AudioParams& params);
    void updateFundamental (const Scribe& scribe);
    

    AudioParams params;

    Range range;
    Threshold threshold;

    Amp amp;
    Delay delay;
    Blocks blocks;

    Shift shift;
    Velocity velocity;

    Note fundamental;

};


inline int getMidiNumber(float freq, float refFreq)
{
    return int(0.5f + 69 + 12 * std::log2(freq / refFreq));
}

inline int midiShift(const Shift& shift, int midiNum)
{
    int midi = midiNum;
    int oct = shift.octave + midi/12;
    int pitch = (midi%12) + shift.semitone;

    int out = 12*oct + pitch;

    if(out < 0 || out > 127)
    {
        return 0;
    }
    
    return out;
}



inline int getVelocity(const Velocity& vel, float dB, float dBFloor) 
{

    //switch this back to previous method with rolling values
    float pct = std::abs((dB - dBFloor) / (vel.maxdB - dBFloor));

    int output = vel.min + pct * (vel.max - vel.min);
    output = output > vel.max ? vel.max : output;
    output = output < vel.min ? vel.min : output;

    return output;
}
