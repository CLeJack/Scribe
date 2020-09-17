#pragma once
#include <JuceHeader.h>

/*
All major sections may have some sub components used for organization
The below list is major components only

ctrl + f #. ClassName

0. Globals
1. GuiParams - parameters that power the note selection algorithm
2. GuiSpectrum - customized frequency spectrum using logic in DCT.cpp
3. GuiLog - print out of all useful info generated within the audioProcessBlock
4. GuiSettings - user settings/saving/loading etc.
5. GuiTabs - holds everything together
*/

//0. GLOBALS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//default x and y values for the project which other screen sizes are based around
//16:9 res
const int REFX = 800; 
const int REFY = 450;

inline void initText(juce::DrawableText& component, const char* text, int justification)
{
    component.setText(text);
    component.setJustification(juce::Justification(justification));
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

class ParamInfo : public juce::Component
{
public:
    ParamInfo();
    void resized() override;

    juce::DrawableText status;
    
};

class ParamNoteID : public juce::Component
{
public:
    ParamNoteID();
    void resized() override;

    juce::DrawableText desc;
    juce::Slider loOct(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    juce::Slider octStr(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    juce::Slider weight(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    juce::Slider retrigger(juce::Slider::SliderStyle::TwoValueVertical, juce::Slider::TextBoxPosition::TextBoxBelow);

    std::vector<juce::Slider*> sliders = {&loOct, &octStr, &weight, &retrigger};
};

class ParamAmplitude : public juce::Component
{
public:
    ParamAmplitude();
    void resized() override;

    juce::DrawableText desc;
    juce::Slider noise(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    juce::Slider release(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    
    std::vector<juce::Slider*> sliders = {&noise, &release};
};

class ParamShift : public juce::Component
{
public:
    ParamShift();
    void resized() override;

    juce::DrawableText desc;
    juce::Slider octave(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    juce::Slider semitone(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    
    std::vector<juce::Slider*> sliders = {&octave, &semitone};
};
class ParamVelocity : public juce::Component
{
public:
    ParamVelocity();
    void resized() override;

    juce::DrawableText desc;
    juce::Slider decibel(juce::Slider::SliderStyle::TwoValueVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    juce::Slider velocity(juce::Slider::SliderStyle::TwoValueVertical, juce::Slider::TextBoxPosition::TextBoxBelow);
    
    std::vector<juce::Slider*> sliders = {&decibel, &velocity};
};

class GuiParams : public juce::Component
{
/*juce::AudioParameterFloat* weightP;
    
    juce::AudioParameterFloat* noiseP;
    juce::AudioParameterFloat* releaseP;
    
    juce::AudioParameterFloat* retrigStartP;
    juce::AudioParameterFloat* retrigStopP;
    
    juce::AudioParameterInt* smoothP;
    
    juce::AudioParameterInt* octaveP;
    juce::AudioParameterInt* semitoneP;
    
    juce::AudioParameterFloat* velDbMinP;
    juce::AudioParameterFloat* velDbMaxP;
    juce::AudioParameterInt* velMinP;
    juce::AudioParameterInt* velMaxP;
    
    juce::AudioParameterInt* channelInP;
    
    juce::AudioParameterInt* loOctP;
    juce::AudioParameterInt* octStrP;*/


public:
    GuiParams();
    ParamInfo info;
    ParamNoteID noteID;
    ParamAmplitude amplitude;
    ParamShift shift;
    ParamVelocity velocity;

    std::vector<juce::Component*> mainSections = {&noteID, &amplitude, &shift, &velocity};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiParams);
};

//1. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiSpectrum : public juce::Component
{
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiSpectrum);
};

//1. GuiWindow ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiWindow : public juce::Component
{
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiWindow);
};

//1. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiLog : public juce::Component
{
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiLog);
};

//1. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiSettings: public juce::Component
{
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiSettings);
};

//1. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiTabs : public juce::TabbedComponent
{
public:

    GuiTabs() : juce::TabbedComponent (juce::TabbedButtonBar::TabsAtTop){};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiTabs);
};

