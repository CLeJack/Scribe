/*
  ==============================================================================

    GUISignal.h
    Created: 1 Oct 2020 2:51:44pm
    Author:  cjack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GUIGlobals.h"
#include "CircularBuffer.h"
#include "Stats.h"



class SignalScope : public juce::Component 
{
public:
    SignalScope(int signalSize);
    void paint(juce::Graphics& g) override;
    std::vector<float> signalVec;
};

class SignalMeter : public juce::Component 
{
public:
    SignalMeter(int bufferSize);
    void paint(juce::Graphics& g) override;

    FloatBuffer dBBuffer;
    
};


class GuiSignal : public juce::Component 
{
public:
    GuiSignal(int signalSize, int bufferSize);
    void resized() override;


    SignalScope scope;

    SignalMeter meter;
    
};