/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================

class ScribeAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ScribeAudioProcessorEditor (ScribeAudioProcessor&);
    ~ScribeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    GUIState getTabState();
    
    inline void updateSpectrum()
    {
        for (int i = 0; i < Scribe::weights.size(); i++)
        {
            guiSpectrum.bars.weights[i] = Scribe::weights[i];
        }
    }

    inline void updateSignal()
    {
        for (int i = 0; i < Scribe::historyDS.size(); i++)
        {
            guiSignal.scope.signalVec[i] = Scribe::historyDS[i];
        }

        guiSignal.meter.dBBuffer.push(Calculations::Amp::dB);

        for (int i = 0; i < guiSignal.meter.thresholds.size(); i++)
        {
            guiSignal.meter.thresholds[i] = weightLimit(
                AudioParams::Threshold::weight,
                AudioParams::Scale::weight,
                AudioParams::Range::lowNote,
                Calculations::Range::lowNote + i * Scribe::Tuning::octaveSize,
                Scribe::Tuning::octaveSize);
        }
    }
    inline void updateMidi() {}
    inline void updateSettings() {}

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