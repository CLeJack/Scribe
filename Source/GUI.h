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



//1. GuiParams ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class GuiParams : public juce::Component, public juce::MouseListener
{

public:
    GuiParams();
    void resized() override;

    char* defaultStatus = "Status";

    juce::Label status;
    SliderPanel noteID{"Note Identification", 5, true};
    SliderPanel amplitude{"Amplitude Thresholds", 2, true};
    SliderPanel midi{"Midi Modifications", 6, true};

    std::vector<SliderPanel*> panels = {&noteID, &amplitude, &midi};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiParams);
};


class GuiWindow : public juce::Component
{
public:
    GuiWindow();
    
    void paint(juce::Graphics& g) override;
    void resized() override;

    FloatBuffer dBBuff;
    std::vector<float> signalVec;

    float currentdB = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiWindow);
};

//3. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiLog : public juce::Component
{
public:
    GuiLog();
    void resized() override;
    void setValueLabels(int f0ind, float f0ratio, int f0oct, int f0pitch,
        int noteInd, float noteRatio, int noteOct, int notePitch,
        float amp, float dB, float trigger, float retrigger,
        int midiOn, int velOn, int midiOff, int velOff);

    std::vector<juce::Label> nameLabels{ 16 };
    std::vector<juce::Label> valueLabels{ 16 };

    juce::Label freqTitle;
    juce::Label ampTitle;
    juce::Label midiTitle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiLog);
};

//4. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiSettings: public juce::Component
{
public:
    GuiSettings();
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiSettings);
};

//5. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiTabs : public juce::TabbedComponent
{
public:

    GuiTabs() : juce::TabbedComponent (juce::TabbedButtonBar::TabsAtTop){};
    void currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName) override;
    GUIState guiState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiTabs);
};

