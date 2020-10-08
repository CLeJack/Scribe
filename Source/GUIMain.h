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

class GuiMain : public juce::Component 
{
    void paint(juce::Graphics& g) override;

    juce::Image logo = juce::ImageFileFormat::loadFrom(BinaryData::scribeicon_png, (size_t)BinaryData::scribeicon_pngSize);
};