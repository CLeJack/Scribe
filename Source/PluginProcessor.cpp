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

    addParameter (loOctP = new juce:: AudioParameterInt ("loNote", "Lowest Note", 0, 3, 2));
    addParameter (octStrP = new juce:: AudioParameterInt ("octStr", "Octave Strength", 0, 5, 3));

    addParameter (noiseP  = new juce::AudioParameterInt("noise", "Noise Floor (dB)", -90, 0, -30));
    addParameter (releaseP  = new juce::AudioParameterInt("release", "Release Floor (dB)", -90, 0, -60));
    
    addParameter (weightP  = new juce::AudioParameterInt("weight", "Weight ", 0, 100, 45));
    addParameter (retrigStartP  = new juce::AudioParameterInt("retrigStart", "Retrigger Start (%)", 80, 120, 90));
    addParameter (retrigStopP  = new juce::AudioParameterInt("retrigStop", "Retrigger Stop (%)", 80, 120, 90));
    
    addParameter (smoothP  = new juce::AudioParameterInt ("smooth", "Detection smoothing", 1, 10, 4));
    
    addParameter (octaveP  = new juce::AudioParameterInt ("octave", "Octave Shift", -8, 8, 0));
    addParameter (semitoneP  = new juce::AudioParameterInt ("semitone", "Semitone Shift", -12, 12, 0));
    
    addParameter (velDbMaxP  = new juce::AudioParameterInt("velDbMax", "Vel dB Max (dB)", -90, 0, -20));
    addParameter (velDbMinP  = new juce::AudioParameterInt("velDbMin", "Vel dB Min (dB)", -90, 0, -30));
    addParameter (velMaxP  = new juce::AudioParameterInt ("velMax", "Vel Max", 0, 127, 96));
    addParameter (velMinP  = new juce::AudioParameterInt ("velMin", "Vel Min", 0, 127, 32));
    
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
    propsPtr.reset (new Properties (getSampleRate(), getBlockSize() ));
    storagePtr.reset ( new Storage (*propsPtr.get() ));
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
    Storage& store = *storagePtr.get();
    Properties& props = *propsPtr.get();
    juce::MidiMessage note;

    AudioParams audioParams = getAudioParams();
    Calculations calcs;


    //add the block to history
    auto* channelData = buffer.getReadPointer(*channelInP);
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        store.history.get()->push(normToInt16Range(channelData[i]));
    }

    //down sample the signal; currently without filtering
    fvec trueSignal = store.history.get()->toOrderedVec();
    fvec signalDS(props.dsHistSamples, 0);
    for (int i = 0; i < signalDS.size(); i++)
    {
        signalDS[i] = trueSignal[i * props.dsFactor];
    }


    calcs.updateRangeInfo(audioParams, signalDS.size());

    fvec weights = dct(*store.matrix.get(), signalDS,
        calcs.loNote, calcs.hiNote, calcs.signalStart, signalDS.size());

    weights = sumNormalize(weights);
    fvec ratios = weightRatio(weights, 12);

    calcs.updateSignalInfo(weights, ratios, signalDS, audioParams);
    calcs.updateMidiNum(store, props, audioParams);

    MidiSwitch& midiSwitch = *store.midiSwitch.get();
    SwitchMessage message{};

    MidiParams midiParams = getMidiParams(calcs, audioParams);

    message = midiSwitch.update(midiParams);

    

    if (message.send)
    {
        note = juce::MidiMessage::noteOff(1, message.off, (juce::uint8) message.offVel);
        midiMessages.addEvent(note, 0);

        note = juce::MidiMessage::noteOn(1, message.on, (juce::uint8)message.onVel);
        midiMessages.addEvent(note, 1);
    }

    frameCounter = (frameCounter + 1) % 30;

    auto editor = (ScribeAudioProcessorEditor*)getActiveEditor();
    if (frameCounter == 0 && editor != nullptr) 
    {
        switch (editor->getTabState())
        {
        case GUIState::spectrum:
            spectrumProcess(weights, calcs);
            break;
        case GUIState::window:
            windowProcess(signalDS, calcs);
            break;
        case GUIState::log:
            logProcess(editor, calcs, message, frameCounter);
            break;
        case GUIState::settings:
            settingsProcess();
            break;
        }
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

void ScribeAudioProcessor::spectrumProcess(const fvec& weights, const Calculations& calcs)
{
}
void ScribeAudioProcessor::windowProcess(const fvec& signal, const Calculations& calcs)
{
}
void ScribeAudioProcessor::logProcess(ScribeAudioProcessorEditor* editor, const Calculations& calcs, const SwitchMessage& message, int frame)
{
        editor->calcs = calcs;
        editor->message = message;
        editor->repaint();
}
void ScribeAudioProcessor::settingsProcess() 
{
}

AudioParams ScribeAudioProcessor::getAudioParams() 
{
    auto output = AudioParams();
    output.loOct = getLoOctP();
    output.octStr = getOctStrP();

    output.noise = getNoiseP();
    output.release = getReleaseP();
    output.weight = getWeightP();
    output.retrigStart = getRetrigStartP();
    output.retrigStop = getRetrigStopP();

    output.smooth = getSmoothP();

    output.octave = getOctaveP();
    output.semitone = getSemitoneP();

    output.velDbMax = getVelDbMaxP();
    output.velDbMin = getVelDbMinP();

    output.velMax = getVelMaxP();
    output.velMin = getVelMinP();

    output.channelIn = getChannelInP();

    return output;
}

MidiParams ScribeAudioProcessor::getMidiParams(const Calculations& calcs, const AudioParams& params)
{
    auto output = MidiParams();
    output.note = calcs.midiNum;
    output.ampdB = calcs.ampdB;
    output.noisedB = params.noise;
    output.releasedB = params.release;
    output.retrig = calcs.retrigger;
    output.retrigStart = params.retrigStart;
    output.retrigStop = params.retrigStop;
    output.velDbMin = params.velDbMin;
    output.velDbMax = params.velDbMax;
    output.velMin = params.velMin;
    output.velMax = params.velMax;
    output.smoothFactor = params.smooth;

    return output;
}