/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessData.h"
#include "GUI.h"


enum class PluginState {waiting, ready, updating};
class ScribeAudioProcessorEditor;
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

    PluginState pluginState;
    

private:
    //==============================================================================
    void initialize();

    void waiting(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void spectrumProcess(const fvec& weights, const Calculations& calcs);
    void windowProcess(const fvec& signal, const Calculations& calcs);
    void logProcess(ScribeAudioProcessorEditor* editor, const Calculations& calcs, const SwitchMessage& message, int frame);
    void settingsProcess();

    std::unique_ptr<Properties> propsPtr;
    std::unique_ptr<Storage> storagePtr;

    std::vector<juce::MidiMessage> notes;
    bool hostInitialized = false;

    int frameCounter = 0;


    

    //Parameters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    juce::AudioParameterInt* loOctP;
    juce::AudioParameterInt* octStrP;

    
    juce::AudioParameterInt* noiseP;
    juce::AudioParameterInt* releaseP;

    juce::AudioParameterInt* weightP;
    
    juce::AudioParameterInt* trigStartP;
    juce::AudioParameterInt* retrigStartP;
    juce::AudioParameterInt* retrigStopP;
    
    juce::AudioParameterInt* smoothP;
    
    juce::AudioParameterInt* octaveP;
    juce::AudioParameterInt* semitoneP;
    
    juce::AudioParameterInt* velDbMinP;
    juce::AudioParameterInt* velDbMaxP;
    juce::AudioParameterInt* velMinP;
    juce::AudioParameterInt* velMaxP;
    
    juce::AudioParameterInt* channelInP;

    inline int getLoOctP() { return *loOctP; }
    inline int getOctStrP() { return *octStrP; }

    inline float getNoiseP() { return (float)*noiseP; }
    inline float getReleaseP() { return (float)*releaseP; }

    inline float getWeightP() { return (float)*weightP / 100.0f; }

    inline float getRetrigStartP() { return (float)*retrigStartP / 100.0f; }
    inline float getRetrigStopP() { return (float)*retrigStopP / 100.0f; }
    inline int getSmoothP() { return *smoothP; }

    inline int getOctaveP() { return *octaveP; }
    inline int getSemitoneP() { return *semitoneP; }

    inline float getVelDbMinP() { return (float)*velDbMinP; }
    inline float getVelDbMaxP() { return (float)*velDbMaxP; }
    inline int getVelMinP() { return *velMinP; }
    inline int getVelMaxP() { return *velMaxP; }
    inline int getChannelInP() { return *channelInP; }

    AudioParams getAudioParams();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScribeAudioProcessor)
};
