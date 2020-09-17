/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScribeAudioProcessorEditor::ScribeAudioProcessorEditor (ScribeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
}

ScribeAudioProcessorEditor::~ScribeAudioProcessorEditor()
{
}

//==============================================================================
void ScribeAudioProcessorEditor::paint (juce::Graphics& g)
{
    if(audioProcessor.getSampleRate()>0)
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

        g.setColour (juce::Colours::white);
        g.setFont (15.0f);
        g.drawFittedText ("test", getLocalBounds(), juce::Justification::centred, 1);
    }
    
}

void ScribeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
