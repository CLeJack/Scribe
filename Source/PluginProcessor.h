/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ModelInstances.h"
#include "GUIGlobals.h"
#include "GUITabs.h"
#include "GUIPaper.h"
#include "GUIMain.h"
#include "GUISpectrum.h"
#include "GUISignal.h"


enum class PluginState {waiting, ready, updating};
class ScribeAudioProcessorEditor;
//==============================================================================
/**
*/
class ScribeAudioProcessor  : public juce::AudioProcessor, public juce::AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    ScribeAudioProcessor();
    ~ScribeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    PluginState pluginState;
    
    //Parameters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    juce::AudioParameterFloat* maxdBP;
    juce::AudioParameterInt* maxVelP;
    juce::AudioParameterInt* minVelP;

    juce::AudioParameterInt* lowNoteP;
    juce::AudioParameterInt* octaveP;
    juce::AudioParameterInt* semitoneP;

    juce::AudioParameterFloat* noiseP;
    juce::AudioParameterBool* bendOnP;
    juce::AudioParameterFloat* bendThresholdP;


private:
    //==============================================================================

    void waiting(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void updateParams();
    

    std::vector<juce::MidiMessage> notes;

    int frameCounter = 0;
    const float fps = 30;
    int fpsBlocks = 11;

    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScribeAudioProcessor)
};
