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
    std::vector<juce::Slider> sliders { 4 };
    std::vector<juce::Label> labels   { 4 };
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

    void paint(juce::Graphics& g) override;

    std::vector<float> relativeHeights{ 4, 0 };
};

class SpectrumBars : public juce::Component 
{
public:
    SpectrumBars(float barCount);
    void paint(juce::Graphics& g);

    std::vector<float> weights;
    float currentWeight;
};

class SpectrumNotes : public juce::Component 
{
public:
    SpectrumNotes(float barCount, float octaveSize);
    //void paint(juce::Graphics& g);

    std::vector<char*> letters = { "C", " ", "D", " ", "E", "F", " ", "G", " ","A", " ", "B" };
    std::vector<char*> panel;
};

class SpectrumdB : public juce::Component 
{
public:
    void paint(juce::Graphics& g);

    float dB = -60;
    float mindB = -60;
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
    SpectrumdB meter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuiSpectrum);
};
