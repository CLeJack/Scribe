#pragma once
#include <JuceHeader.h>
#include "GUIGlobals.h"
#include "Stats.h"
#include "CircularBuffer.h"

/*
All major sections may have some sub components used for organization
The below list is major components only

ctrl + f #. ClassName

0. General GUI
1. GuiParams - parameters that power the note selection algorithm
2. GuiSpectrum - customized frequency spectrum using logic in DCT.cpp
3. GuiWindow - View into the window used to generate GuiSpectrum and amplitude data
3. GuiLog - print out of all useful info generated within the audioProcessBlock
4. GuiSettings - user settings/saving/loading etc.
5. GuiTabs - holds everything together
*/

//0. General GUI ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~





inline void initText(juce::Label& component, const char* text, int justification)
{
    component.setText(text, juce::NotificationType::dontSendNotification);
    component.setJustificationType(juce::Justification(justification));
}

inline void resizeH(juce::Rectangle<int>& area, int pixels) 
{
    area.removeFromTop(pixels);
    area.removeFromBottom(pixels);
}

inline void resizeW(juce::Rectangle<int>& area, float pixels) 
{
    area.removeFromLeft(pixels);
    area.removeFromRight(pixels);
}

inline void resizeHW(juce::Rectangle<int>& area, float pixh, float pixw)
{
    resizeH(area, pixh);
    resizeW(area, pixw);
}



//5. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiTabs : public juce::TabbedComponent
{
public:

    GuiTabs() : juce::TabbedComponent (juce::TabbedButtonBar::TabsAtTop){};
    void currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName) override;
    GUIState guiState = GUIState::main;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiTabs);
};

class PaperLookAndFeel : public juce::LookAndFeel_V4 
{
public:
    PaperLookAndFeel();
};

