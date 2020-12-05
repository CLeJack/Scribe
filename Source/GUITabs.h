/*
  ==============================================================================

    GUITabs.h
    Created: 4 Dec 2020 11:44:58am
    Author:  cjack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "GUIGlobals.h"
#include "Stats.h"
#include "CircularBuffer.h"

class GuiTabs : public juce::TabbedComponent
{
public:

    GuiTabs() : juce::TabbedComponent (juce::TabbedButtonBar::TabsAtTop){};
    void currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName) override;
    GUIState guiState = GUIState::main;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiTabs);
};