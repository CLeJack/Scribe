#pragma once
#include <JuceHeader.h>

/*
All major sections may have some sub components used for organization
The below list is major components only

ctrl + f #. ClassName

0. Globals
1. GuiParams - parameters that power the note selection algorithm
2. GuiSpectrum - customized frequency spectrum using logic in DCT.cpp
3. GuiWindow - View into the window used to generate GuiSpectrum and amplitude data
3. GuiLog - print out of all useful info generated within the audioProcessBlock
4. GuiSettings - user settings/saving/loading etc.
5. GuiTabs - holds everything together
*/

//0. GLOBALS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//default x and y values for the project which other screen sizes are based around
//16:9 res
const int REFX = 800; 
const int REFY = 450;

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

class ParamNoteID : public juce::Component
{
public:
    ParamNoteID();
    void resized() override;

    juce::Label desc;

    std::vector<juce::Slider> sliders{ 4 };

    std::vector<juce::Range<double>> ranges = {
        juce::Range<double>(0,3),
        juce::Range<double>(0,5),
        juce::Range<double>(0,100),
        juce::Range<double>(80,120) };

    std::vector<double> intervals = {1, 0.5, 0.5, 1};

    std::vector<juce::Label> labels{ 4 };
    std::vector<const char*> labelText = { "Low Oct", "Oct Str", "Wght", "Retrig" };
    
    std::vector<const char*> hoverTips = { 
        "Lowest Octave: ", 
        "Octave Strength: ", 
        "Note Selection Weight: ", 
        "Retrigger Pct: " };
};

class ParamAmplitude : public juce::Component
{
public:
    ParamAmplitude();
    void resized() override;

    juce::Label desc;
    
    std::vector<juce::Slider> sliders{ 2 };

    std::vector<juce::Range<double>> ranges = {
        juce::Range<double>(-90,0),
        juce::Range<double>(-90,0) };

    std::vector<double> intervals = { 1, 1 };


    std::vector<juce::Label> labels{ 2 };
    std::vector<const char*> labelText = { "Noise", "Rel"};

    std::vector<const char*> hoverTips = {
        "Noise Threshold: ",
        "Release Threshold: "};
};

class ParamShift : public juce::Component
{
public:
    ParamShift();
    void resized() override;

    juce::Label desc;
    
    std::vector<juce::Slider> sliders{ 2 };

    std::vector<juce::Range<double>> ranges = {
        juce::Range<double>(-8,8),
        juce::Range<double>(-11,11) };

    std::vector<double> intervals = { 1, 1 };


    std::vector<juce::Label> labels{ 2 };
    std::vector<const char*> labelText = { "Oct", "Semi" };

    std::vector<const char*> hoverTips = {
        "Octave Shift: ",
        "Semitone Shift: " };
};
class ParamVelocity : public juce::Component
{
public:
    ParamVelocity();
    void resized() override;

    juce::Label desc;
    
    std::vector<juce::Slider> sliders{ 2 };

    std::vector<juce::Range<double>> ranges = {
        juce::Range<double>(-90,0),
        juce::Range<double>(0, 127) };

    std::vector<double> intervals = { 1, 1 };


    std::vector<juce::Label> labels{ 2 };
    std::vector<const char*> labelText = { "dB", "Vel" };

    std::vector<const char*> hoverTips = {
        "Decibel Range: ",
        "Velocity Range: " };
};

class GuiParams : public juce::Component, public juce::MouseListener
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
    void resized() override;
    void addSliderListeners(std::vector<juce::Slider>& sliders);
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    char* defaultStatus = "Status";

    juce::Label status;
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
public:
    GuiSpectrum();
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiSpectrum);
};

//2. GuiWindow ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiWindow : public juce::Component
{
public:
    GuiWindow();
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiWindow);
};

//3. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiLog : public juce::Component
{
public:
    GuiLog();
    void resized() override;
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiTabs);
};

