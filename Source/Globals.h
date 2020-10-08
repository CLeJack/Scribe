#pragma once
#include "Head.h"

namespace Calculations
{

    extern float weight;
    extern float retrigger;

    namespace Range 
    {
        extern int lowNote;
        extern int highNote;
    }

    namespace Fundamental 
    {
        extern int index;
        extern int octave;
        extern int pitch;
        extern float ratio;
    }

    namespace Note 
    {
        extern int index;
        extern int octave;
        extern int pitch;
        extern float ratio;
    }

    namespace Threshold 
    {
        extern float weight;

        extern float noise;

        extern float release;
        extern float retrigger;
    }

    //amplitude
    namespace Amp 
    {
        extern float amp  ; //formerly ampFull
        extern float half1;
        extern float half2;
        extern float dB   ; //formerly ampdB
    }

    namespace Delay 
    {
        extern float amp;   //formerly velocityAmp
        extern float dB ; //formerly attackdB
    }

    namespace Blocks
    {
        extern float midi;
        extern float amp;
        extern float dB;    
    }

    namespace Midi 
    {
        extern int index; //formerly midiNum
        extern int velocity;
    }

    namespace Angle 
    {
        // angle is used for velocity calculations
        // it measures how quickly a value in the Amp name space changes
        // w.r.t. a corresponding Delay variable
        extern float amp;
        extern float dB;
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace AudioParams 
{
    namespace Range 
    {
        extern int lowNote;
        extern int highNote;
    }
    
    namespace Threshold 
    {
        extern int ratio;

        extern int release;

        extern float weight0;
        extern float weight1;
        extern float weight2;
        extern float weight3;

        extern float noise0;
        extern float noise1;
        extern float noise2;
        extern float noise3;

        extern float retrigStart;
        extern float retrigStop;
    }

    namespace Scale 
    {
        extern int noise;
        extern int weight;
    }
    

    namespace SmoothTime //corresponds with Calculation Delay 
    {
        //these were initially block sizes
        //They will all change to milliseconds
        extern int midi; //formerly midiSmooth
        extern int dB; //formerly attack smooth
        extern int amp; //formerly velocity smooth
    }
    
    namespace Shift 
    {
        extern int octave;
        extern int semitone;
    }

    
    namespace Angle 
    {
        // min angle will be considered 45 
        // atan of a smoothed signal vs the current value will output
        // approx 45 when both are equal
        extern float dB;
        extern float amp;
    }

    namespace Velocity
    {
        extern int min;
        extern int max;
    }

    extern int channelIn;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
struct Range 
{
    int lowNote = 28;
    int highNote = lowNote + 48;
};

struct Threshold
{
    float ratio;

    float release;

    float weight0;
    float weight1;
    float weight2;
    float weight3;

    float noise0;
    float noise1;
    float noise2;
    float noise3;

    float retrigStart;
    float retrigStop;
}
*/