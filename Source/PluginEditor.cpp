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
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (REFX, REFY);

    addAndMakeVisible(guiTabs);
    auto colour = findColour (juce::ResizableWindow::backgroundColourId);

    guiTabs.addTab("Params",   colour, &guiParams,   false);
    guiTabs.addTab("Spectrum", colour, &guiSpectrum, false);
    guiTabs.addTab("Window",   colour, &guiWindow,   false);
    guiTabs.addTab("Log",      colour, &guiLog,      false);
    guiTabs.addTab("Settings", colour, &guiSettings, false);

    guiParams.resized();
    guiSpectrum.resized();
    guiWindow.resized();
    guiLog.resized();
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
        case GUIState::log:
            paintLog();
            break;
        case GUIState::spectrum:
            paintSpectrum();
            break;
        case GUIState::window:
            paintWindow();
            break;
    }
    
}

void ScribeAudioProcessorEditor::resized()
{
    guiTabs.setBounds (getLocalBounds());
}


void ScribeAudioProcessorEditor::paintLog() 
{
    guiLog.setValueLabels(calcs.f0ind, calcs.f0ratio, calcs.f0oct, calcs.f0pitch,
        calcs.noteInd, calcs.noteRatio, calcs.noteOct, calcs.notePitch,
        calcs.ampFull, calcs.ampdB, calcs.retrigger, calcs.retrigger,
        message.on, message.onVel, message.off, message.offVel);
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
        guiSpectrum.data[i - loNote] = weights[i];
    }

    guiSpectrum.weight = weight;

}

void ScribeAudioProcessorEditor::updateWindow(const std::vector<float>& signal, float ampdB) 
{
    for (int i = 0; i < std::min(guiWindow.signalVec.size(), signal.size()); i++) 
    {
        guiWindow.signalVec[i] = signal[i];
    }
    guiWindow.dBBuff.push(ampdB);
    guiWindow.currentdB = ampdB;
}