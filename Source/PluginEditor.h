/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================

class ScribeAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    ScribeAudioProcessorEditor (ScribeAudioProcessor&);
    ~ScribeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override ;
    
    void updateSpectrum();
    void updateSignal();
    void updateMidi(bool send);
    void updateSettings();

    void setSliders();
    GUIState getTabState();
    
    
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ScribeAudioProcessor& audioProcessor;

    GuiMain guiMain;
    GuiSpectrum guiSpectrum;
    GuiSignal guiSignal;
    GuiMidi guiMidi;
    GuiSettings guiSettings;
    GuiTabs guiTabs;
    

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScribeAudioProcessorEditor)
};