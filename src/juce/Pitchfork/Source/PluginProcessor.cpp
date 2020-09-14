/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PitchforkAudioProcessor::PitchforkAudioProcessor()
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
    
    addParameter (channelInP = new juce::AudioParameterInt ("channel", "Active Channel",0,1,1));
}

PitchforkAudioProcessor::~PitchforkAudioProcessor()
{
}

//==============================================================================
const juce::String PitchforkAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PitchforkAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PitchforkAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PitchforkAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PitchforkAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PitchforkAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PitchforkAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PitchforkAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PitchforkAudioProcessor::getProgramName (int index)
{
    return {};
}

void PitchforkAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PitchforkAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PitchforkAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PitchforkAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PitchforkAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    
    juce::ScopedNoDenormals noDenormals;
    auto iChanCount  = getTotalNumInputChannels();
    auto oChanCount = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    //buffer.clear (i, 0, buffer.getNumSamples());
    
    std::cout<<getSampleRate()<< ", "<< oChanCount<< ", "<< iChanCount<< ", "<< getBlockSize()<<"\n";
    
    
    switch(state)
    {
        case PluginState::waiting :
            std::cout<<"waiting\n";
            waiting (buffer, midiMessages);
            break;
        case PluginState::ready :
            std::cout<<"ready\n";    
            ready (buffer, midiMessages);
            break;
        case PluginState::updating :
            std::cout<<"updating\n";
            updating (buffer, midiMessages);
            break;
    }
    
    buffer.clear();
    
}

//==============================================================================
bool PitchforkAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PitchforkAudioProcessor::createEditor()
{
    return new PitchforkAudioProcessorEditor (*this);
}

//==============================================================================
void PitchforkAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PitchforkAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchforkAudioProcessor();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// custom functions

void PitchforkAudioProcessor::initialize()
{
    propsPtr.reset (new Properties (getSampleRate(), getBlockSize() ));
    storagePtr.reset ( new Storage (*propsPtr.get() ));
}

void PitchforkAudioProcessor::waiting(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if(getSampleRate() > 0)
    {
        initialize();
        state = PluginState::ready;
    }
    buffer.clear();
}

void PitchforkAudioProcessor::ready(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    Storage& S = *storagePtr.get();
    Properties& P = *propsPtr.get();

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
    
    int windowStart = signalDS.size() - signalDS.size()/2 ;
    
    fvec weights = dct (*S.matrix.get(), signalDS, 0, P.freqSize, windowStart, signalDS.size());
    
    weights = sumNormalize(weights);
    
    fvec comy2 = CoMY2(signalDS);
}

void PitchforkAudioProcessor::updating(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    Storage& storage = *storagePtr.get();
    Properties& props = *propsPtr.get();
    
    initialize();
    state = PluginState::ready;
    
    buffer.clear();
}
