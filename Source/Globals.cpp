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
        int lowNote = 28;
        int highNote = 0;
    }
    
    namespace Threshold 
    {
        int ratio = 3;

        int release = -60;

        float noise0 = -30;
        float noise1 = -35;
        float noise2 = -40;
        float noise3 = -45;

        float weight0 = 0.05f;
        float weight1 = 0.05f;
        float weight2 = 0.10f;
        float weight3 = 0.15f;

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
        //these were milliseconds, but the behavior is strange; recheck calculation
        //currently using block size which
        int midi = 4; //formerly midiSmooth
        int dB = 4; //formerly attack smooth
        int amp = 4; //formerly velocity smooth
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
        float dB = 75;
        float amp = 75;
    }

    namespace Velocity
    {
        int min = 50;
        int max = 127;
    }

    int channelIn = 0;
}