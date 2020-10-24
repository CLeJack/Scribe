/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScribeAudioProcessorEditor::ScribeAudioProcessorEditor(ScribeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    guiSpectrum(scribe.frequencies.size(), scribe.tuning.octaveSize),
    guiSignal(scribe.audio.ds.samples, 60)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (REFX, REFY);

    setLookAndFeel(new PaperLookAndFeel());

    guiMainTab.setLookAndFeel(new PaperLookAndFeel());
    guiMainPanel.setLookAndFeel(new PaperLookAndFeel());
    guiTabs.setLookAndFeel(new PaperLookAndFeel());
    guiSpectrum.setLookAndFeel(new PaperLookAndFeel());
    guiSignal.setLookAndFeel(new PaperLookAndFeel());

    addAndMakeVisible(guiMainPanel);
    addAndMakeVisible(guiTabs);

    guiTabs.addTab("Main",   PAPER, &guiMainTab,   false);
    guiTabs.addTab("Spectrum", PAPER, &guiSpectrum, false);
    guiTabs.addTab("Signal", PAPER, &guiSignal,   false);

    guiMainTab.resized();
    guiMainPanel.resized();
    guiSpectrum.resized();
    guiSignal.resized();

    guiMainPanel.maxdB.slider.addListener(this);
    guiMainPanel.maxVel.slider.addListener(this);
    guiMainPanel.minVel.slider.addListener(this);
    
    guiMainPanel.lowNote.slider.addListener(this);
    guiMainPanel.octave.slider.addListener(this);
    guiMainPanel.semitone.slider.addListener(this);
    
    guiMainPanel.noise.slider.addListener(this);

    guiMainPanel.panic.addListener(this);
    


    setSliders();
    

}

ScribeAudioProcessorEditor::~ScribeAudioProcessorEditor()
{
}

//==============================================================================
void ScribeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
}

void ScribeAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    guiMainPanel.setBounds(area.removeFromLeft(area.getWidth() * .333));
    guiTabs.setBounds (area);
}




GUIState ScribeAudioProcessorEditor::getTabState() 
{
    return guiTabs.guiState;
}

void ScribeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
    auto& panel = guiMainPanel;

    if (slider == &panel.minVel.slider)
    {
        params.velocity.min = slider->getValue();
    }

    else if (slider == &panel.maxVel.slider)
    {
        params.velocity.max = slider->getValue();
    }
    else if (slider == &panel.maxdB.slider)
    {
        params.velocity.maxdB = slider->getValue();
    }

    else if (slider == &panel.octave.slider)
    {
        params.shift.octave = slider->getValue();
    }

    else if (slider == &panel.semitone.slider)
    {
        params.shift.semitone = slider->getValue();
    }
    
    else if (slider == &panel.noise.slider)
    {
        params.threshold.noise = slider->getValue();
    }
    else if (slider == &panel.lowNote.slider)
    {
        params.range.lowNote = slider->getValue();
    }



}

void ScribeAudioProcessorEditor::buttonClicked(juce::Button* button) 
{
    if (button == &guiMainPanel.panic) 
    {
        scribe.sendAllNotesOff = true;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ScribeAudioProcessorEditor::setSliders()
{
    guiMainPanel.maxdB.slider.setValue(params.velocity.maxdB);
    guiMainPanel.maxVel.slider.setValue(params.velocity.max);
    guiMainPanel.minVel.slider.setValue(params.velocity.min);

    guiMainPanel.lowNote.slider.setValue(params.range.lowNote);
    guiMainPanel.octave.slider.setValue(params.shift.octave);
    guiMainPanel.semitone.slider.setValue(params.shift.semitone);

    guiMainPanel.noise.slider.setValue(params.threshold.noise);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ScribeAudioProcessorEditor::updateSpectrum()
{
    for (int i = 0; i < scribe.weights.size(); i++)
    {
        guiSpectrum.certainty.weights[i] = scribe.fundamentalHistory[i];
    }


}

void ScribeAudioProcessorEditor::updateSignal()
{
    for (int i = 0; i < scribe.historyDS.size(); i++)
    {
        guiSignal.scope.signalVec[i] = scribe.historyDS[i];
    }

    guiSignal.meter.dBBuffer.push(calcs.amp.dB);

}
