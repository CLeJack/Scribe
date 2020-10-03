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

class SignalSliders : public juce::Component
{
public:
    SignalSliders();
    void resized() override;
    std::vector<juce::Slider> sliders;
    std::vector<juce::Label> labels;
};



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
    SignalMeter(int bufferSize, int thresholdSize);
    void paint(juce::Graphics& g) override;

    fvec thresholds;
    FloatBuffer dBBuffer;
    
};

class SignalThresholds : public juce::Component
{
public:
    SignalThresholds();
    void paint(juce::Graphics& g) override;

    std::vector<float> relativeHeights;
};

class GuiSignal : public juce::Component 
{
public:
    GuiSignal(int signalSize, int bufferSize, int thresholdSize);
    void resized() override;

    SignalSliders sliderPanel;
    SignalScope scope;

    SignalThresholds thresholds;
    SignalMeter meter;
    
};