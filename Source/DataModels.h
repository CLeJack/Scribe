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
    
    float retrig = 0.98f;
    float retrigSameNote = 0.90f;
    float retrigStop  = 1.1f;

    float consistency = .95f;

    // deactivation threshold; lowervalues make the pitch state turn off more easily
    // going lower than .04 barely allows for pitcbending
    float bend = .06; 
};

struct Scale
{
    float noise  = -5;
    float weight = 0.5f;
};

struct SmoothTime //corresponds with Calculation Delay 
{
    //milliseconds
    float midi     = .025f;
    float dBShort  = .005f; //higher frequency events
    float dBLong   = .010f; //lower frequency events
};

struct Shift
{
    int octave   = 0;
    int semitone = 0;
};

struct Velocity
{
    int min = 0;
    int max = 127;

    float maxdB = 0;
    int current = 0;
};

//calculation specific

enum class TriggerState { trigger, retriggering, stable };

struct Amp
{
    float val   = 0;
    float half1 = 0;
    float half2 = 0;
    float dB    = 0;
    float dBPrev = 0;
    float retrig = 0;
    float slope = 0;
};

struct Delay
{
    float dbShortPrev = -90;
    float dBShort = -90;
    float dBLong  = -90;
};

struct Blocks
{
    float midi = 1;
    float dBShort  = 1;
    float dBLong   = 1;
};

//for SMA function (simple moving average)
struct Fraction
{
    float midi = 0;
    float dBShort = 0;
    float dBLong = 0;
};

struct Note
{
    int index       = 0;
    int prevIndex   = 0;
    float history   = 0;

    int lastActivated = 0;
};

struct Consistency
{
    float history = 0;
    float current = 0;
    bool isConsistent = false;
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
    
    void updateFundamental(const Range& range, const Blocks& blocks);


    bool isInitialized = false; //don't run PluginProcessor ready state loop without this set to true

    Tuning tuning;
    Audio audio;

    fvec frequencies = fvec(1 + tuning.highExp - tuning.lowExp, 0);
    ivec midiNumbers = ivec(frequencies.size(), 0);

    fvec weights        = fvec(frequencies.size(), 0);
    fvec maxWeights     = fvec(frequencies.size(), 0);
    fvec sumWeights     = fvec(frequencies.size(), 0);

    fvec fundamentalCertainty = fvec(frequencies.size(), 0);
    fvec fundamentalHistory   = fvec(frequencies.size(), 0);

    Note fundamental;
    bool inTriggerState = false;
    float dBSmoothing = 0;

    fvec timeVector = fvec(audio.ds.samples, 0);

    bool sendAllNotesOff = false;

    cmatrix matrix = cmatrix(frequencies.size(), cvec(audio.ds.samples, std::complex<float>(0, 0)));

    //sum normalization and max normalization
    fmatrix maxOctMatrix = fmatrix(frequencies.size(), fvec(audio.ds.samples,0));
    fmatrix maxSineMatrix = fmatrix(frequencies.size(), fvec(audio.ds.samples,0));


    std::unique_ptr<Buffer<float> > history;

    fvec historyDS = fvec(audio.ds.samples, 0.0001f);

    MidiSwitch midiSwitch;

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
    bool bendOn = false;
};

// 3. Calculations Data Model~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Calculations
{
    //these functions should be called in order
    void updateRange  (const Scribe& scribe, const AudioParams& params);
    void updateSignal (const Scribe& scribe, const AudioParams& params);
    void updateConsistency(const Scribe& scribe, const AudioParams& params);
    void updatePitchWheel(const Scribe& scribe, const AudioParams& params);
    

    AudioParams params;

    Range range;
    Threshold threshold;

    Amp amp;
    Delay delay;
    Fraction fraction;
    Blocks blocks;

    Shift shift;
    Velocity velocity;

    Note fundamental;

    Consistency consistency;

    float pitchWheelPosition = 0;
    float frequencyDelta = 0; //for triggering the pitch wheel

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

MidiParams getMidiParams(const Calculations& calcs, Scribe& scribe, const AudioParams& params);
