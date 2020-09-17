/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI.h"

//==============================================================================
/**
*/
class ScribeAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ScribeAudioProcessorEditor (ScribeAudioProcessor&);
    ~ScribeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    GuiParams guiParams;
    GuiSpectrum guiSpectrum;
    GuiWindow guiWindow;
    GuiLog guiLog;
    GuiSettings guiSettings;
    GuiTabs guiTabs;



private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ScribeAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScribeAudioProcessorEditor)
};
