/*
  ==============================================================================

    GUIPaper.h
    Created: 4 Dec 2020 11:45:30am
    Author:  cjack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GUIGlobals.h"
#include "Stats.h"
#include "CircularBuffer.h"

class PaperLookAndFeel : public juce::LookAndFeel_V4 
{
public:
    PaperLookAndFeel();
};