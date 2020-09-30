/*
  ==============================================================================

    Calculations.h
    Created: 30 Sep 2020 11:50:37am
    Author:  cjack

  ==============================================================================
*/

#pragma once



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
