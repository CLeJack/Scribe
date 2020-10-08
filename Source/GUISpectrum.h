/*
  ==============================================================================

    GUISpectrum.h
    Created: 1 Oct 2020 2:51:19pm
    Author:  cjack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GUIGlobals.h"
#include "Stats.h"

class SpectrumSliders : public juce::Component 
{
public:
    SpectrumSliders();
    void resized() override;
    std::vector<juce::Slider> sliders;
    std::vector<juce::Label> labels;
};

class SpectrumBackground : public juce::Component 
{
public:
    SpectrumBackground(float barCount, float octaveSize);

    void paint(juce::Graphics& g) override;
    
    float barCount;
    float octaveSize;

};

class SpectrumThresholds : public juce::Component 
{
public:
    SpectrumThresholds();
    void paint(juce::Graphics& g) override;

    std::vector<float> relativeHeights;
};

class SpectrumBars : public juce::Component 
{
public:
    SpectrumBars(float barCount);
    void paint(juce::Graphics& g) override;

    std::vector<float> weights;
};

class SpectrumNotes : public juce::Component 
{
public:
    SpectrumNotes(float barCount, float octaveSize);
    void paint(juce::Graphics& g) override;

    std::vector<int> keyReference = { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
    std::vector<int> keys;
};


//1. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiSpectrum : public juce::Component
{
public:
    GuiSpectrum(int barCount, int octaveSize);
    void resized() override;

    SpectrumBackground background;
    SpectrumSliders sliderPanel;
    SpectrumThresholds thresholds;
    SpectrumNotes notes;
    SpectrumBars bars;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuiSpectrum);
};
