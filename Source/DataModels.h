#pragma once
#include "Head.h"
#include "Waveforms.h"
#include "Tuning.h"
#include "CircularBuffer.h"
#include "Stats.h"
#include "DCT.h"
#include "ProcessData.h"
#include "MidiSwitch.h"

/*
* 
1. Scribe Model
2. AudioParam Model
3. Calculations Model

*/

// 1. Scribe program data model~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
    DownSample ds;
};

//~~~~~~~~~~~~~~~~~~~~~~
struct Scribe {

    void initialize(float srate, float blockSize);//set all required non-const variables here
    bool detectsPropertyChange(float srate, float blockSize);
    bool isInitialized = false; //don't run PluginProcessor ready state loop without this set to true

    Tuning tuning;
    Audio audio;

    fvec frequencies = fvec(1 + tuning.highExp - tuning.lowExp, 0);

    fvec weights = fvec(frequencies.size(), 0);
    fvec ratios = fvec(frequencies.size(), 0);

    fvec timeVector = fvec(audio.ds.samples, 0);

    cmatrix matrix = cmatrix(frequencies.size(), cvec(audio.ds.samples, std::complex<float>(0, 0)));

    std::unique_ptr<FloatBuffer> history;

    fvec historyDS = fvec(audio.ds.samples, 0.0001f);

    MidiSwitch midiSwitch = MidiSwitch();
};



// 2. AudioParam data model~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Range 
{
    int lowNote  = 28;
    int highNote = lowNote + 48;
};

struct Threshold
{
    float ratio = 3;

    float release = -60;

    float weight0 = 0.05f;
    float weight1 = 0.05f;
    float weight2 = 0.10f;
    float weight3 = 0.15f;

    float noise0 = -35;
    float noise1 = -40;
    float noise2 = -45;
    float noise3 = -50;

    float retrigStart = 0.9f;
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
    float midi     = 11;
    float dBShort  = 11; //shorter smoothing time
    float dBLong   = dBShort * 2; //longer smoothing time
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

    float ratio = .15;
};

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

struct RangeResults
{
    int lowNote  = 0;
    int highNote = 0;
};

struct ThresholdResults
{
    float weight = 0;
    float noise  = 0;

    float release   = 0;
    float retrigger = 0;

    float retrigStart = 0;
    float retrigStop  = 0;
};

struct Note
{
    int index   = 0;
    int octave  = 0;
    int pitch   = 0;
    float ratio = 0;
};

struct Midi
{
    int index    = 0;
    int velocity = 0;
};

struct Amp
{
    float val   = 0;
    float half1 = 0;
    float half2 = 0;
    float dB    = 0;
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



struct Targets 
{
    float weight    = 0;
    float retrigger = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Calculations
{
    //these functions should be called in order
    void updateRange  (const Range& params);
    void updateSignal (const Scribe& scribe, const AudioParams& params);
    void updateMidi   (const Scribe& scribe, const AudioParams& params);
    AudioParams params;

    RangeResults range;
    ThresholdResults threshold;

    Amp amp;
    Delay delay;
    Blocks blocks;
    
    Note fundamental;
    Note note;
    Midi midi;

    Targets targets;

};

MidiParams getMidiParams(const Calculations& calcs);
