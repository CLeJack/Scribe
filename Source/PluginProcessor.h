/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessData.h"

enum class PluginState {waiting, ready, updating};
//==============================================================================
/**
*/
class ScribeAudioProcessor  : public juce::AudioProcessor
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

private:
    //==============================================================================

    std::unique_ptr<Properties> propsPtr;
    std::unique_ptr<Storage> storagePtr;

    juce::AudioParameterFloat* weightP;
    
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
    juce::AudioParameterInt* octStrP;
    
    std::vector<juce::MidiMessage> notes;
    bool hostInitialized = false;


    void initialize();
    
    void waiting(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    
    
    
    PluginState state = PluginState::waiting;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScribeAudioProcessor)
};
