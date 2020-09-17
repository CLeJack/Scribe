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
    setSize (REFX, REFY);

    addAndMakeVisible(guiTabs);
    auto colour = findColour (juce::ResiableWindow::backgroundColourId);

    guiTabs.addTab("Params",   colour, guiParams.get(),   false);
    guiTabs.addTab("Spectrum", colour, guiSpectrum.get(), false);
    guiTabs.addTab("Window",   colour, guiWindow.get(),   false);
    guiTabs.addTab("Log",      colour, guiLog.get(),      false);
    guiTabs.addTab("Settings", colour, guiSettings.get(), false);
    
}

ScribeAudioProcessorEditor::~ScribeAudioProcessorEditor()
{
}

//==============================================================================
void ScribeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("test", getLocalBounds(), juce::Justification::centred, 1);
    
}

void ScribeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
