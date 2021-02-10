/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#pragma once


#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
ScribeAudioProcessor::ScribeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(maxdBP = new juce::AudioParameterFloat("maxdBP", "Max dB", -60.0f, 0.0f, params.velocity.maxdB));
    addParameter(maxVelP = new juce::AudioParameterInt("maxVelP", "Max Vel.", 0, 127, params.velocity.max));
    addParameter(minVelP = new juce::AudioParameterInt("minVelP", "Min Vel.", 0, 127, params.velocity.min));

    addParameter(lowNoteP = new juce::AudioParameterInt("lowNoteP", "Low Note", 12, 28, params.range.lowNote));
    addParameter(octaveP = new juce::AudioParameterInt("octaveP", "Octave", -8, 8, params.shift.octave));
    addParameter(semitoneP = new juce::AudioParameterInt("semitoneP", "Semitone", -12, 12, params.shift.semitone));

    addParameter(noiseP = new juce::AudioParameterFloat("noiseP", "Noise (dB)", -60.0f, 0.0f, params.threshold.noise));
    addParameter(bendOnP = new juce::AudioParameterBool("bendOnP", "Pitch Bend", true));
    addParameter(bendThresholdP = new juce::AudioParameterFloat("bendThresholdP", "Bend Threshold", 0.0f, 1.0f, params.threshold.bend));

    maxdBP->addListener(this);
    maxVelP->addListener(this);
    minVelP->addListener(this);

    lowNoteP->addListener(this);
    octaveP->addListener(this);
    semitoneP->addListener(this);

    noiseP->addListener(this);

    pluginState = PluginState::waiting;
    
}

ScribeAudioProcessor::~ScribeAudioProcessor()
{
}

//==============================================================================
const juce::String ScribeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ScribeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ScribeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ScribeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ScribeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ScribeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ScribeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ScribeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ScribeAudioProcessor::getProgramName (int index)
{
    return {};
}

void ScribeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ScribeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ScribeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ScribeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ScribeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    switch(pluginState)
    {
        case PluginState::ready :   
            ready (buffer, midiMessages);
            break;
        case PluginState::updating :
            updating (buffer, midiMessages);
            break;
        default :
            waiting(buffer, midiMessages);
            break;
    }

    if (scribe.sendAllNotesOff) 
    {
        juce::MidiMessage note;

        for (int i = 0; i < 128; i++)
        {
            note = juce::MidiMessage::noteOff(1, i, (juce::uint8)0);
            midiMessages.addEvent(note, 0);
        }
        scribe.sendAllNotesOff = false;
    }
    
}

//==============================================================================
bool ScribeAudioProcessor::hasEditor() const
{
    //GUILESS is defined in projucer exporter preprocessor definitions
#if GUILESS == 0

    return  true;

#else

    return false;
#endif
}

juce::AudioProcessorEditor* ScribeAudioProcessor::createEditor()
{

#if GUILESS == 0

    return  new ScribeAudioProcessorEditor(*this);

#else

    return nullptr;
#endif
}

//==============================================================================
void ScribeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ScribeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ScribeAudioProcessor();
}

void ScribeAudioProcessor::parameterValueChanged(int parameterIndex, float newValue) 
{
    updateParams();
}

void ScribeAudioProcessor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom functions


//plugin state processing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ScribeAudioProcessor::waiting(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if(getSampleRate() > 0)
    {
        scribe.initialize(getSampleRate(), getBlockSize());
        pluginState = PluginState::ready;
    }
    buffer.clear();
}

void ScribeAudioProcessor::ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    fpsBlocks = getSampleRate() / (getBlockSize() * fps);
    //add the block to history
    auto* channelData = buffer.getReadPointer(0);
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        scribe.history.get()->push(normToInt16Range(channelData[i]));
    }

    //down sample the signal; currently without filtering
    fvec trueSignal = scribe.history.get()->toOrderedVec();
    
    for (int i = 0; i < scribe.historyDS.size(); i++)
    {
        scribe.historyDS[i] = trueSignal[i * scribe.audio.ds.factor];
    }

    updateParams(); //align virtualParams with gui

    calcs.updateRange(scribe, params);

    calcs.updateSignal(scribe, params);

    scribe.updateFundamental(calcs.range, calcs.blocks);

    calcs.updateConsistency(scribe, params);

    calcs.updatePitchWheel(scribe, params);


    SwitchMessage message{};

    MidiParams midiParams = getMidiParams(calcs, scribe, params);

    message = scribe.midiSwitch.update(midiParams);
    
    juce::MidiMessage note;
    
    if (message.send)
    {
        
        note = juce::MidiMessage::noteOff(1, message.off, (juce::uint8) message.offVel);
        midiMessages.addEvent(note, 0);

        note = juce::MidiMessage::noteOn(1, message.on, (juce::uint8)message.onVel);
        midiMessages.addEvent(note, 0);
    }

    if (calcs.pitchWheelPosition > 0.001 && params.bendOn == true) 
    {
        note = juce::MidiMessage::pitchWheel(1, 16383 * (0.5 * calcs.pitchWheelPosition + 0.5));

        midiMessages.addEvent(note, 0);
    }
    
    
    frameCounter = (frameCounter + 1) % fpsBlocks; 

    auto editor = (ScribeAudioProcessorEditor*)getActiveEditor();
    
    if (frameCounter == 0 && editor != nullptr) 
    {
        //const juce::MessageManagerLock mmLock;

        switch (editor->getTabState()) 
        {
        case GUIState::spectrum:
            editor->updateSpectrum();
            break;
        case GUIState::signal:
            editor->updateSignal();
            break;
        case GUIState::main:
            break;
        }
        editor->repaint();
        
    }
    

    buffer.clear();
}

void ScribeAudioProcessor::updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    pluginState = PluginState::waiting;
    
    buffer.clear();
}

void ScribeAudioProcessor::updateParams() 
{
    params.velocity.maxdB   = *maxdBP;
    params.velocity.max     = *maxVelP;
    params.velocity.min     = *minVelP;
    params.range.lowNote    = *lowNoteP;
    params.shift.octave     = *octaveP;
    params.shift.semitone   = *semitoneP;
    params.threshold.noise  = *noiseP;
    params.bendOn           = *bendOnP;
    params.threshold.bend = *bendThresholdP;
}

//Gui state processing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~