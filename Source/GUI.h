#pragam once
#include <JuceHeader.h>

/*
All major sections may have some sub components used for organization
The below list is major components only

ctrl + f #. ClassName

1. GuiParams - parameters that power the note selection algorithm
2. GuiSpectrum - customized frequency spectrum using logic in DCT.cpp
3. GuiLog - print out of all useful info generated within the audioProcessBlock
4. GuiSettings - user settings/saving/loading etc.
5. GuiTabs - holds everything together
*/

//GLOBALS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//default x and y values for the project which other screen sizes are based around
//16:9 res
const int REFX 800; 
const int REFY 450;

//1. GuiParams ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ParamInfo : public juce::Component
{
public:
    ParamInfo();
    juce::DrawableText status;
    
};

class ParamSelection : public juce::Component
{
public:
    ParamSelection();
    juce::Slider loOct(juce::SliderStyle LinearBarVertical, juce::TextBoxPosition TextBoxBelow);
    juce::Slider octStr(juce::SliderStyle LinearBarVertical, juce::TextBoxPosition TextBoxBelow);
    juce::Slider weight(juce::SliderStyle LinearBarVertical, juce::TextBoxPosition TextBoxBelow);
    juce::Slider retrigger(juce::SliderStyle TwoValueVertical, juce::TextBoxPosition TextBoxBelow);
};

class ParamAmplitude : public juce::Component
{
public:
    ParamAmplitude();
    juce::Slider noise(juce::SliderStyle LinearBarVertical, juce::TextBoxPosition TextBoxBelow);
    juce::Slider release(juce::SliderStyle LinearBarVertical, juce::TextBoxPosition TextBoxBelow);
};

class ParamShift : public juce::Component
{
public:
    ParamShift();
    juce::Slider octave(juce::SliderStyle LinearBarVertical, juce::TextBoxPosition TextBoxBelow);
    juce::Slider semitone(juce::SliderStyle LinearBarVertical, juce::TextBoxPosition TextBoxBelow);
};
class ParamVelocity : public juce::Component
{
public:
    ParamVelocity();
    juce::Slider decibel(juce::SliderStyle TwoValueVertical, juce::TextBoxPosition TextBoxBelow);
    juce::Slider velocity(juce::SliderStyle TwoValueVertical, juce::TextBoxPosition TextBoxBelow);
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
    ParamSelection selection;
    ParamAmplitude amplitude;
    ParamShift shift;
    ParamVelocity velocity;
};

//1. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiSpectrum : public juce::Component{};

//1. GuiWindow ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiWindow : public juce::Component{};

//1. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiLog : public juce::Component{};

//1. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiSettings: public juce::Component{};

//1. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class GuiTabs : public juce::TabbedComponent( juce::TabbedButtonBar::Orientation TabsAtTop)
{
public:
    GuiParams guiParams;
    GuiSpectrum guiSpectrum;
    GuiWindow guiWindow;
    GuiLog guiLog;
    GuiSettings guiSettings;
};

