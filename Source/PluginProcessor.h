/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ProcessData.h"
#include "GUI.h"
#include "GUIGlobals.h"
#include "GUIMain.h"
#include "GUISpectrum.h"
#include "GUISignal.h"
#include "GUIMidi.h"


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
    
    //Parameters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    juce::AudioParameterInt* ratioP;
    juce::AudioParameterInt* lowNoteP;

    juce::AudioParameterInt* noiseP;
    juce::AudioParameterInt* noiseScaleP;

    juce::AudioParameterInt* releaseP;

    juce::AudioParameterInt* weightP;
    juce::AudioParameterInt* weightScaleP;

    juce::AudioParameterInt* trigStartP;
    juce::AudioParameterInt* retrigStartP;
    juce::AudioParameterInt* retrigStopP;

    juce::AudioParameterInt* midiSmoothP;
    juce::AudioParameterInt* ampSmoothP;
    juce::AudioParameterInt* dBSmoothP;

    juce::AudioParameterInt* octaveP;
    juce::AudioParameterInt* semitoneP;

    juce::AudioParameterFloat* maxAngleP;
    juce::AudioParameterInt* velMinP;
    juce::AudioParameterInt* velMaxP;

    juce::AudioParameterInt* channelInP;

    /*
    inline int getRatioP() { return *ratioP; }
    inline int getLowNoteP() { return *lowNoteP; }

    inline float getNoiseP() { return (float)*noiseP; }
    inline float getReleaseP() { return (float)*releaseP; }

    inline float getWeightP() { return (float)*weightP / 1000.0f; }

    inline float getRetrigStartP() { return (float)*retrigStartP / 100.0f; }
    inline float getRetrigStopP() { return (float)*retrigStopP / 100.0f; }
    inline int getMidiSmoothP() { return *midiSmoothP; }
    inline int getAmpSmoothP() { return *ampSmoothP; }
    inline int getdBSmoothP() { return *dBSmoothP; }

    inline int getOctaveP() { return *octaveP; }
    inline int getSemitoneP() { return *semitoneP; }

    inline float getMaxAngleP() { return *maxAngleP; }
    inline int getVelMinP() { return *velMinP; }
    inline int getVelMaxP() { return *velMaxP; }
    inline int getChannelInP() { return *channelInP; }

    */


private:
    //==============================================================================
    void initialize();
    void updateAudioParams();


    void waiting(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void spectrumProcess(ScribeAudioProcessorEditor* editor);
    void windowProcess(ScribeAudioProcessorEditor* editor);
    void logProcess(ScribeAudioProcessorEditor* editor, const SwitchMessage& message, int frame);
    void settingsProcess();
    

    std::vector<juce::MidiMessage> notes;

    int frameCounter = 0;


    

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScribeAudioProcessor)
};
