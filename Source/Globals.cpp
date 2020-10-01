#include "Globals.h"

namespace Calculations
{

    float weight = 0;
    float retrigger = 0;

    namespace Range 
    {
        int lowNote = 0;
        int highNote = 0;
    }

    namespace Fundamental 
    {
        int index = 0;
        int octave = 0;
        int pitch = 0;
        float ratio = 0;
    }

    namespace Note 
    {
        int index = 0;
        int octave = 0;
        int pitch = 0;
        float ratio = 0;
    }

    namespace Threshold 
    {
        float weight = 0;
        float noise = 0;
        float release = 0;
        float retrigger = 0;
    }

    //amplitude
    namespace Amp 
    {
        float amp   = 0; //formerly ampFull
        float half1 = 0;
        float half2 = 0;
        float dB    = -60; //formerly ampdB
    }

    namespace Delay 
    {
        float amp = 0;   //formerly velocityAmp
        float dB  = -60; //formerly attackdB
    }

    namespace Blocks
    {
        float midi = 0;
        float amp = 0;
        float dB = 0;
    }

    namespace Midi 
    {
        int index = 0; //formerly midiNum
        int velocity = 0;
    }

    namespace Angle 
    {
        // angle is used for velocity calculations
        // it measures how quickly a value in the Amp name space changes
        // w.r.t. a corresponding Delay variable
        float amp = 45;
        float dB = 45;
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace AudioParams 
{
    namespace Range 
    {
        int lowNote = 0;
        int highNote = 0;
    }
    
    namespace Threshold 
    {
        int ratio = 3;

        int release = -50;

        int noise = -35;

        float weight = 10;

        float retrigStart = 0.9f;
        float retrigStop = 1.0f;
    }

    namespace Scale 
    {
        int noise = -5;
        int weight = 66;
    }
    

    namespace SmoothTime //corresponds with Calculation Delay 
    {
        //these were initially block sizes
        //They will all change to milliseconds
        int midi = 11; //formerly midiSmooth
        int dB = 11; //formerly attack smooth
        int amp = 11; //formerly velocity smooth
    }
    
    namespace Shift 
    {
        int octave = 0;
        int semitone = 0;
    }

    
    namespace Angle 
    {
        // min angle will be considered 45 
        // atan of a smoothed signal vs the current value will output
        // approx 45 when both are equal
        float dB = 45;
        float amp = 45;
    }

    namespace Velocity
    {
        int min = 0;
        int max = 0;
    }

    int channelIn = 0;
}