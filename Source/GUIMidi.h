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

class MidiBar : public juce::Component 
{
public:
    MidiBar(float min, float max, float ticks);
    void paint(juce::Graphics& g) override;
    float val;
    float min;
    float max;
    float ticks;
};

class MidiData : public juce::Component 
{
public:
    void paint(juce::Graphics& g) override;
    float midi;
    float vel;
};

class MidiSliders : public juce::Component
{
public:
    MidiSliders();
    void resized() override;
    std::vector<juce::Slider> sliders;
    std::vector<juce::Label> labels;
};

class GuiMidi : public juce::Component 
{
public:
    GuiMidi();
    void resized() override;
    void paint(juce::Graphics& g);


    MidiSliders sliderPanel;
    //MidiBar angle;
    //MidiBar dB;

    //MidiData on;
    //MidiData off;

    int delayedAmp = 0;
    float angle = 0;

    float midiOn = 0;
    float velOn = 0;

    float retrigger = 0;
};