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

    addParameter (ratioP = new juce:: AudioParameterInt ("ratio", "Octave Ratio", 0, 5, 3));
    addParameter(lowNoteP = new juce::AudioParameterInt("lowNote", "Lowest Note", 12, 36, 28));

    addParameter (noiseP  = new juce::AudioParameterInt("noise", "Noise Floor (dB)", -90, 0, -30));
    addParameter(noiseScaleP = new juce::AudioParameterInt("noiseScale", "Noise Floor Scaling (dB/oct)", -90, 0, -30));

    addParameter (releaseP  = new juce::AudioParameterInt("release", "Release Floor (dB)", -90, 0, -50));
    
    addParameter (weightP  = new juce::AudioParameterInt("weight", "Weight Threshold", 0, 100, 45));
    addParameter(weightScaleP = new juce::AudioParameterInt("weightScale", "Weight Threshold Scaling", 1, 100, 66));

    addParameter (retrigStartP  = new juce::AudioParameterInt("retrigStart", "Retrigger Start", 50, 120, 90));
    addParameter (retrigStopP  = new juce::AudioParameterInt("retrigStop", "Retrigger Stop", 50, 120, 100)); 
    
    addParameter (midiSmoothP  = new juce::AudioParameterInt ("midiSmooth", "Midi smoothing (ms)", 0, 25, 11));
    addParameter(ampSmoothP = new juce::AudioParameterInt("ampSmooth", "Amp Smoothing (ms)", 0, 25, 11));
    addParameter(dBSmoothP = new juce::AudioParameterInt("dBSmooth", "dB Smoothing (ms)", 0, 25, 11));
    
    addParameter (octaveP  = new juce::AudioParameterInt ("octave", "Octave Shift", -8, 8, 0));
    addParameter (semitoneP  = new juce::AudioParameterInt ("semitone", "Semitone Shift", -12, 12, 0));
    
    addParameter(maxAngleP = new juce::AudioParameterFloat("maxAngle", "Max Amplitude Angle (deg)", 45.0f, 90.0f, 50.0f));
    addParameter (velMaxP  = new juce::AudioParameterInt ("velMax", "Vel Max", 0, 127, 100));
    addParameter (velMinP  = new juce::AudioParameterInt ("velMin", "Vel Min", 0, 127, 40));
    
    addParameter (channelInP  = new juce::AudioParameterInt ("channelIn", "Input Channel", 0,  1, 1) );

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

void ScribeAudioProcessor::initialize()
{
    Scribe::initialize(getSampleRate(), getBlockSize() );
}

void ScribeAudioProcessor::updateAudioParams() 
{
    namespace A = AudioParams;
    
    A::Threshold::ratio = getRatioP();
    
    A::Range::lowNote = getLowNoteP();


    A::Threshold::release = getReleaseP();

    A::Threshold::noise  = getNoiseP();
    A::Threshold::weight = getWeightP();

    A::Threshold::retrigStart = getRetrigStartP();
    A::Threshold::retrigStop  = getRetrigStopP();

    A::SmoothTime::midi = getMidiSmoothP();
    A::SmoothTime::amp  = getAmpSmoothP();
    A::SmoothTime::dB   = getdBSmoothP();

    A::Shift::octave   = getOctaveP();
    A::Shift::semitone = getSemitoneP();


    A::Angle::amp    = getMaxAngleP();
    A::Velocity::max = getVelMaxP();
    A::Velocity::min = getVelMinP();

    A::channelIn = getChannelInP();
}

void ScribeAudioProcessor::waiting(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if(getSampleRate() > 0)
    {
        initialize();
        pluginState = PluginState::ready;
    }
    buffer.clear();
}

void ScribeAudioProcessor::ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    namespace S = Scribe;
    namespace A = AudioParams;
    namespace C = Calculations;

    juce::MidiMessage note;

    updateAudioParams();

    //add the block to history
    auto* channelData = buffer.getReadPointer(0);
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        S::history.get()->push(normToInt16Range(channelData[i]));
    }

    //down sample the signal; currently without filtering
    fvec trueSignal = S::history.get()->toOrderedVec();
    
    for (int i = 0; i < S::historyDS.size(); i++)
    {
        S::historyDS[i] = trueSignal[i * S::DownSample::factor];
    }


    updateRangeCalcs();

    //discrete customized transform (dft) using a portion of frequency and signal
    dct(S::weights,
        S::matrix,  //dft using this matrix
        S::historyDS, //on this signal
        C::Range::lowNote, C::Range::highNote, //for these rows
        S::DownSample::signalStart, S::historyDS.size()); //and these columns

    sumNormalize(S::weights);
    weightRatio(S::ratios, S::weights, 12);

    //I need void version of the above for pre existing matrices;

    updateSignalCalcs();

    updateMidiCalcs();

    SwitchMessage message{};

    MidiParams midiParams = getMidiParams();

    message = S::midiSwitch.update(midiParams);

    

    if (message.send)
    {
        note = juce::MidiMessage::noteOff(1, message.off, (juce::uint8) message.offVel);
        midiMessages.addEvent(note, 0);

        note = juce::MidiMessage::noteOn(1, message.on, (juce::uint8)message.onVel);
        midiMessages.addEvent(note, 1);
    }

    //don't forget to update this to be srate specific
    // 11 was with 44100 hz in mind and is approximately 30 fps
    frameCounter = (frameCounter + 1) % 11; 

    frameCounter = message.send ? 0 : frameCounter;
    auto editor = (ScribeAudioProcessorEditor*)getActiveEditor();
    
    if (frameCounter == 0 && editor != nullptr) 
    {
        //const juce::MessageManagerLock mmLock;
        //editor->repaint();
        //send data--not sure if repaint is needed
        switch (editor->getTabState()) 
        {
        case GUIState::spectrum:
            editor->updateSpectrum();
            break;
        case GUIState::signal:
            editor->updateSignal();
            break;
        case GUIState::midi:
            editor->updateMidi();
            break;
        case GUIState::settings:
            editor->updateSettings();
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
    
    initialize();
    pluginState = PluginState::ready;
    
    buffer.clear();
}

//Gui state processing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~