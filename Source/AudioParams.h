/*
  ==============================================================================

    ScribeParams.h
    Created: 30 Sep 2020 11:50:11am
    Author:  cjack

  ==============================================================================
*/

#pragma once
namespace AudioParams 
{
    int lowNote = 0;
    
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
