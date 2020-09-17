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

//1. GuiParams ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

    GuiParams
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

