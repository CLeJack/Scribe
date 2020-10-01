/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


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
    GUIState getTabState();
    void updateSpectrum(const std::vector<float>& weights, int loNote, float weight);
    void updateWindow(const std::vector<float>& signal, float ampdB);

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

    void paintLog();
    void paintSpectrum();
    void paintWindow();
    

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScribeAudioProcessorEditor)
};