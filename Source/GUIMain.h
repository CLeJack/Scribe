/*
  ==============================================================================

    GUIMain.h
    Created: 1 Oct 2020 2:52:15pm
    Author:  cjack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GUIGlobals.h"

class GuiMainTab : public juce::Component 
{
    void paint(juce::Graphics& g) override;

    juce::Image logo = juce::ImageFileFormat::loadFrom(BinaryData::scribeicon_png, (size_t)BinaryData::scribeicon_pngSize);
};

class GuiMainSlider : public juce::Component 
{
public:
    GuiMainSlider(const char* text, bool showText, double min, double max, double step);
    void resized() override;
    void paint(juce::Graphics& g) override;
    juce::Slider slider;
    juce::Label text;
    const bool showText;
};

class GuiMainPanel : public juce::Component 
{
public:
    GuiMainPanel();
    void resized() override;
    juce::Label spectrumLabel;
    juce::Label midiLabel;

    GuiMainSlider maxVel;
    GuiMainSlider minVel;

    GuiMainSlider maxdB;
    GuiMainSlider noise;

    GuiMainSlider lowNote;
    GuiMainSlider octave;
    GuiMainSlider semitone;

    juce::TextButton panic;
};