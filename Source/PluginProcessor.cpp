/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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

    addParameter (loOctP = new juce:: AudioParameterInt ("loNote", "Lowest Note", 12, 84, 24));
    addParameter (octStrP = new juce:: AudioParameterInt ("octStr", "Octave Strength", 0, 5, 3));

    addParameter (noiseP  = new juce::AudioParameterFloat ("noise", "Noise Floor (dB)", -90, 0, -30));
    addParameter (releaseP  = new juce::AudioParameterFloat ("release", "Release Floor (dB)", -90, 0, -60));
    addParameter (weightP  = new juce::AudioParameterFloat ("weight", "Weight ", 0.045f, 0.1f, 0.045f));
    
    addParameter (retrigStartP  = new juce::AudioParameterFloat ("retrigStart", "Retrigger Start (%)", 0.80, 1.0f, 0.9f));
    addParameter (retrigStopP  = new juce::AudioParameterFloat ("retrigStop", "Retrigger Stop (%)", 0.80, 1.2f, 0.99f));
    addParameter (smoothP  = new juce::AudioParameterInt ("smooth", "Detection smoothing", 1, 10, 4));
    
    addParameter (octaveP  = new juce::AudioParameterInt ("octave", "Octave Shift", -8, 8, 0));
    addParameter (semitoneP  = new juce::AudioParameterInt ("semitone", "Semitone Shift", -12, 12, 0));
    
    addParameter (velDbMaxP  = new juce::AudioParameterFloat ("velDbMax", "Vel dB Max (dB)", -90, 0, -20));
    addParameter (velDbMinP  = new juce::AudioParameterFloat ("velDbMin", "Vel dB Min (dB)", -90, 0, -30));
    addParameter (velMaxP  = new juce::AudioParameterInt ("velMax", "Vel Max", 0, 127, 96));
    addParameter (velMinP  = new juce::AudioParameterInt ("velMin", "Vel Min", 0, 127, 32));
    
    addParameter (channelInP  = new juce::AudioParameterInt ("channelIn", "Input Channel", 0,  1, 1) );
    
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
    

    switch(state)
    {
        case PluginState::waiting :
            waiting (buffer, midiMessages);
            break;
        case PluginState::ready :   
            ready (buffer, midiMessages);
            break;
        case PluginState::updating :
            updating (buffer, midiMessages);
            break;
    }
    
}

//==============================================================================
bool ScribeAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ScribeAudioProcessor::createEditor()
{
    //return new ScribeAudioProcessorEditor (*this);
    return nullptr;
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
        state = PluginState::ready;
    }
    buffer.clear();
}

void ScribeAudioProcessor::ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    Storage& S = *storagePtr.get();
    Properties& P = *propsPtr.get();
    juce::MidiMessage note;

    auto* channelData = buffer.getReadPointer (*channelInP);
    for(int i = 0; i < buffer.getNumSamples(); i++)
    {
        S.history.get()->push (normToInt16Range(channelData[i]));
    }
    
    fvec trueSignal = S.history.get()->toOrderedVec();
    fvec signalDS ( P.dsHistSamples,0);

    for(int i = 0; i < signalDS.size(); i++)
    {
        signalDS[i] = trueSignal[i*P.dsFactor];
    }
    
    fvec weights = dct (*S.matrix.get(), signalDS, *loNoteP, *hiNoteP, 0, signalDS.size());
    
    weights = sumNormalize(weights);
    fvec ratios = weightRatio(weights, 12);
    int f0ind = maxArg(weights);

    int octaveThresh = 3;
    if(ratios[f0ind] < octaveThresh)
    {
        f0ind = f0ind-12 < 0 ? 0 : f0ind - 12;
    }

    fvec comy2 = CoMY2(signalDS);

    float avgPeakAmp = 0.5*(maxValue(signalDS) - minValue(signalDS));

    float metricTrigger= weights[f0ind];
    float metricRetrigger = comy2[1]/comy2[0];
    float amp = int16ToDb(avgPeakAmp);


    f0ind = metricTrigger < *weightP ? 0 : f0ind;
    f0ind = f0ind < 24 ? 0 : f0ind;
    f0ind = amp < *noiseP ? 0 : f0ind;
    

    float midiNum = 0;
    MidiSwitch& midiSwitch = *S.midiSwitch.get();
    SwitchMessage message{};

    midiNum = midiShift(f0ind, *S.frequencies.get(), P.refFreq, *octaveP, *semitoneP);
    MidiParams p{midiNum, amp, *noiseP, *releaseP, 
    metricRetrigger, *retrigStartP, *retrigStopP, 
    *velDbMinP, *velDbMaxP, *velMinP, *velMaxP,
    *smoothP};
    message = midiSwitch.update(p);
    
    if(message.send)
    {
        note = juce::MidiMessage::noteOff( 1, message.off, (juce::uint8) message.offVel);
        midiMessages.addEvent(note,0);

        note = juce::MidiMessage::noteOn( 1, message.on, (juce::uint8)message.onVel);
        midiMessages.addEvent(note,1);
    }

    buffer.clear();

}

void ScribeAudioProcessor::updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    initialize();
    state = PluginState::ready;
    
    buffer.clear();
}