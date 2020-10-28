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

        for (int i = 0; i < scribe.fOnNotes.size(); i++)
        {
            note = juce::MidiMessage::noteOff(1, scribe.finalNote[i], (juce::uint8)0);
            midiMessages.addEvent(note, 0);
            scribe.turnOffMidi(i);
        }
        scribe.sendAllNotesOff = false;
    }
    
}

//==============================================================================
bool ScribeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ScribeAudioProcessor::createEditor()
{
    return new ScribeAudioProcessorEditor (*this);
    //return nullptr;
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


    calcs.updateRange(scribe, params);

    calcs.updateSignal(scribe, params);

    scribe.updateFundamental(calcs.range, calcs.blocks, calcs.amp, calcs.threshold);
    //chords currently aren't functioning
    //try the pitch-octave-certainty implementation next
    //scribe.updateChords(calcs.range, calcs.blocks, calcs.amp, calcs.threshold);

    //scribe.updateFMidiInfo(calcs.threshold, calcs.amp, calcs.velocity, calcs.range, calcs.shift, calcs.blocks);
    //scribe.updateCMidiInfo(calcs.threshold, calcs.amp, calcs.velocity, calcs.range, calcs.shift);

    //processMidi(midiMessages);

    SwitchMessage message{};

    MidiParams midiParams = getMidiParams(calcs, scribe);

    message = scribe.midiSwitch.update(midiParams);

    if (message.send)
    {
        juce::MidiMessage note;
        note = juce::MidiMessage::noteOff(1, message.off, (juce::uint8) message.offVel);
        midiMessages.addEvent(note, 0);

        note = juce::MidiMessage::noteOn(1, message.on, (juce::uint8)message.onVel);
        midiMessages.addEvent(note, 1);
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

void ScribeAudioProcessor::processMidi(juce::MidiBuffer& midiMessages)
{
    juce::MidiMessage note;

    for (int i = 0; i < scribe.fOnNotes.size(); i++) 
    {
        if (scribe.fNeedsTrigger[i]) 
        {
            note = juce::MidiMessage::noteOn(1, scribe.finalNote[i], (juce::uint8) calcs.velocity.current);
            midiMessages.addEvent(note, 0);
            scribe.turnOnMidi(i, calcs.amp, calcs.threshold);
        }

        //calc dB check is itentional redundancy; this should turn off all notes if they are on when the amplitude is too low
        if (scribe.fNeedsRelease[i]) 
        {
            note = juce::MidiMessage::noteOff(1, scribe.finalNote[i], (juce::uint8)0);
            midiMessages.addEvent(note, 0);
            scribe.turnOffMidi(i);
        }
    }
}

void ScribeAudioProcessor::updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    pluginState = PluginState::waiting;
    
    buffer.clear();
}

//Gui state processing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~