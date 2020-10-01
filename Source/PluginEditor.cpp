/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScribeAudioProcessorEditor::ScribeAudioProcessorEditor (ScribeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), guiSpectrum(Scribe::frequencies.size(), Scribe::Tuning::octaveSize)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (REFX, REFY);

    setLookAndFeel(new PaperLookAndFeel());
    addAndMakeVisible(guiTabs);
    auto colour = findColour (juce::ResizableWindow::backgroundColourId);

    guiTabs.addTab("Main",   colour, &guiMain,   false);
    guiTabs.addTab("Spectrum", colour, &guiSpectrum, false);
    guiTabs.addTab("Signal",   colour, &guiSignal,   false);
    guiTabs.addTab("Midi",      colour, &guiMidi,      false);
    guiTabs.addTab("Settings", colour, &guiSettings, false);

    guiMain.resized();
    guiSpectrum.resized();
    guiSignal.resized();
    guiMidi.resized();
    guiSettings.resized();

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
    switch (guiTabs.guiState) 
    {
        case GUIState::main:
            
            break;
        case GUIState::spectrum:
            //paintSpectrum();
            break;
        case GUIState::signal:
            //paintWindow();
            break;
    }
    
}

void ScribeAudioProcessorEditor::resized()
{
    guiTabs.setBounds (getLocalBounds());
}


void ScribeAudioProcessorEditor::paintLog() 
{
}
void ScribeAudioProcessorEditor::paintSpectrum()
{
}
void ScribeAudioProcessorEditor::paintWindow() {}

GUIState ScribeAudioProcessorEditor::getTabState() 
{
    return guiTabs.guiState;
}

void ScribeAudioProcessorEditor::updateSpectrum(const std::vector<float>& weights, int loNote, float weight)
{
    for (int i = loNote; i < loNote + 48; i++) 
    {
        guiSpectrum.bars.weights[i - loNote] = weights[i];
    }

    guiSpectrum.meter.dB = Calculations::Amp::dB;

    float minWeight = AudioParams::Threshold::weight;
    float weightScale = AudioParams::Scale::weight;
    float minInd = Calculations::Range::lowNote;

    for (int i = 0; i < guiSpectrum.thresholds.relativeHeights.size(); i++) 
    {
        guiSpectrum.thresholds.relativeHeights[i] = weightLimit(minWeight, weightScale, minInd, i * 12 + minInd, Scribe::Tuning::octaveSize);
    }
    

}

void ScribeAudioProcessorEditor::updateWindow(const std::vector<float>& signal, float ampdB) 
{
    //for (int i = 0; i < std::min(guiWindow.signalVec.size(), signal.size()); i++) 
    //{
    //    guiWindow.signalVec[i] = signal[i];
    //}
    //guiWindow.dBBuff.push(ampdB);
    //guiWindow.currentdB = ampdB;
}