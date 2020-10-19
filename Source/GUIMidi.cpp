/*
  ==============================================================================

    GUIMidi.cpp
    Created: 1 Oct 2020 2:52:04pm
    Author:  cjack

  ==============================================================================
*/

#include "GUIMidi.h"


GuiMidi::GuiMidi() : dB(5, -60, 0), vel(5, 0 , 127)
{
    addAndMakeVisible(dB);
    addAndMakeVisible(vel);
}

void GuiMidi::resized() 
{
    auto area = getLocalBounds();
    float pad = area.getWidth() * 0.05f;

    REMOVE_FROM_ALL_SIDES(area, pad);

    midiBars.setBounds(area);
    
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MidiBars::MidiBars(int barCount) : SpectrumBars(barCount)
{
}

void MidiBars::paint(juce::Graphics& g) 
{
    g.setColour(BOLD_BLACK_INK);
    g.drawRect(getLocalBounds(), 2);
}