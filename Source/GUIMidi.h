/*
  ==============================================================================

    GUIMidi.h
    Created: 1 Oct 2020 2:52:04pm
    Author:  cjack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GUIGlobals.h"


class MidiMeter : public juce::Component 
{
public:
    MidiMeter(int ticks, float min, float max);
    void paint(juce::Graphics& g) override;
  
    int ticks = 1;
    const float scaleMax;
    const float scaleMin;
    float refMax = 0;
    float refCurrent = 0;
    float refMin = 0;
};

class GuiMidi : public juce::Component 
{
public:
    GuiMidi();
    void resized() override;

    MidiMeter dB;
    MidiMeter vel;
};