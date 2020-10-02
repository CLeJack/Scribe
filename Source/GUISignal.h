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

class SignalScope : public juce::Component 
{
    void paint(juce::Graphics& g) override;
};

class SignaldB : public juce::Component 
{
    void paint(juce::Graphics& g) override;

    fvec thresholds;
    
};

class GuiSignal : public juce::Component 
{
    GuiSignal();
    void resized() override;
};