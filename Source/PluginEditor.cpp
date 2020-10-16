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
    guiSignal(scribe.audio.ds.samples, 60, 4)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (REFX, REFY);

    setLookAndFeel(new PaperLookAndFeel());

    guiMain.setLookAndFeel(new PaperLookAndFeel());
    guiTabs.setLookAndFeel(new PaperLookAndFeel());
    guiSpectrum.setLookAndFeel(new PaperLookAndFeel());
    guiSignal.setLookAndFeel(new PaperLookAndFeel());
    guiMidi.setLookAndFeel(new PaperLookAndFeel());
    guiSettings.setLookAndFeel(new PaperLookAndFeel());

    addAndMakeVisible(guiTabs);

    guiTabs.addTab("Main",   PAPER, &guiMain,   false);
    guiTabs.addTab("Spectrum", PAPER, &guiSpectrum, false);
    guiTabs.addTab("Signal", PAPER, &guiSignal,   false);
    guiTabs.addTab("Midi", PAPER, &guiMidi,      false);
    guiTabs.addTab("Settings", PAPER, &guiSettings, false);

    guiMain.resized();
    guiSpectrum.resized();
    guiSignal.resized();
    guiMidi.resized();
    guiSettings.resized();

    CALL_EACH_ELEM_FUNC(guiSpectrum.sliderPanel.sliders, addListener(this));
    CALL_EACH_ELEM_FUNC(guiSignal.sliderPanel.sliders, addListener(this));
    CALL_EACH_ELEM_FUNC(guiMidi.sliderPanel.sliders, addListener(this));

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




GUIState ScribeAudioProcessorEditor::getTabState() 
{
    return guiTabs.guiState;
}

void ScribeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
    auto &spectrum = guiSpectrum.sliderPanel.sliders;
    auto &signal = guiSignal.sliderPanel.sliders;
    auto &midi = guiMidi.sliderPanel.sliders;

    
    if (slider == &spectrum[4])
    {
        params.range.lowNote = slider->getValue();
    }
    

    else if (slider == &midi[0])
    {
        params.smoothTime.dBShort = slider->getValue();
    }
    else if (slider == &midi[1])
    {
        params.smoothTime.dBLong = slider->getValue();
    }
 
    else if (slider == &midi[3])
    {
        params.velocity.min = slider->getValue();
    }

    else if (slider == &midi[4])
    {
        params.velocity.max = slider->getValue();
    }

    else if (slider == &midi[5])
    {
        params.shift.octave = slider->getValue();
    }

    else if (slider == &midi[6])
    {
        params.shift.semitone = slider->getValue();
    }

    else if (slider == &midi[7])
    {
        params.smoothTime.midi = slider->getValue();
    }

    else if (slider == &midi[8])
    {
        params.threshold.retrig = slider->getValue();
    }

    else if (slider == &midi[9])
    {
        params.threshold.retrigStop = slider->getValue();
    }


}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ScribeAudioProcessorEditor::setSliders()
{
    //Spectrum panel
    guiSpectrum.sliderPanel.labels[0].setText("Weight 0", juce::NotificationType::dontSendNotification);
    guiSpectrum.sliderPanel.labels[1].setText("Weight 1", juce::NotificationType::dontSendNotification);
    guiSpectrum.sliderPanel.labels[2].setText("Weight 2", juce::NotificationType::dontSendNotification);
    guiSpectrum.sliderPanel.labels[3].setText("Weight 3", juce::NotificationType::dontSendNotification);

    guiSpectrum.sliderPanel.labels[4].setText("Low Note", juce::NotificationType::dontSendNotification);
    guiSpectrum.sliderPanel.labels[5].setText("Octave Ratio", juce::NotificationType::dontSendNotification);

    guiSpectrum.sliderPanel.sliders[0].setRange(0, 0.5, 0.002);
    guiSpectrum.sliderPanel.sliders[1].setRange(0, 0.5, 0.002);
    guiSpectrum.sliderPanel.sliders[2].setRange(0, 0.5, 0.002);
    guiSpectrum.sliderPanel.sliders[3].setRange(0, 0.5, 0.002);

    guiSpectrum.sliderPanel.sliders[4].setRange(12, 28, 1);
    guiSpectrum.sliderPanel.sliders[5].setRange(0, 10, 1);

    //guiSpectrum.sliderPanel.sliders[0].setValue(params.threshold.weight0);
    //guiSpectrum.sliderPanel.sliders[1].setValue(params.threshold.weight1);
    //guiSpectrum.sliderPanel.sliders[2].setValue(params.threshold.weight2);
    //guiSpectrum.sliderPanel.sliders[3].setValue(params.threshold.weight3);
    guiSpectrum.sliderPanel.sliders[4].setValue(params.range.lowNote);
    //guiSpectrum.sliderPanel.sliders[5].setValue(params.threshold.ratio);


    //signal panel
    guiSignal.sliderPanel.labels[0].setText("Noise 0", juce::NotificationType::dontSendNotification);
    guiSignal.sliderPanel.labels[1].setText("Noise 1", juce::NotificationType::dontSendNotification);
    guiSignal.sliderPanel.labels[2].setText("Noise 2", juce::NotificationType::dontSendNotification);
    guiSignal.sliderPanel.labels[3].setText("Noise 3", juce::NotificationType::dontSendNotification);

    guiSignal.sliderPanel.sliders[0].setRange(-60, 0, 1);
    guiSignal.sliderPanel.sliders[1].setRange(-60, 0, 1);
    guiSignal.sliderPanel.sliders[2].setRange(-60, 0, 1);
    guiSignal.sliderPanel.sliders[3].setRange(-60, 0, 1);

    //guiSignal.sliderPanel.sliders[0].setValue(params.threshold.noise0);
    //guiSignal.sliderPanel.sliders[1].setValue(params.threshold.noise1);
    //guiSignal.sliderPanel.sliders[2].setValue(params.threshold.noise2);
    //guiSignal.sliderPanel.sliders[3].setValue(params.threshold.noise3);


    //midi panel
    guiMidi.sliderPanel.labels[0].setText("Short Delay ", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[1].setText("Long Delay", juce::NotificationType::dontSendNotification);

    guiMidi.sliderPanel.labels[2].setText("Vel Ratio", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[3].setText("Min Vel.", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[4].setText("Max Vel.", juce::NotificationType::dontSendNotification);

    guiMidi.sliderPanel.labels[5].setText("Oct. Shift", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[6].setText("Semi Shift", juce::NotificationType::dontSendNotification);

    guiMidi.sliderPanel.labels[7].setText("Midi Delay", juce::NotificationType::dontSendNotification);
    
    guiMidi.sliderPanel.labels[8].setText("Retrigger Start", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[9].setText("Retrigger Stop", juce::NotificationType::dontSendNotification);


    guiMidi.sliderPanel.sliders[0].setRange(0, 25, 1);
    guiMidi.sliderPanel.sliders[1].setRange(0, 25, 1);

    guiMidi.sliderPanel.sliders[2].setRange(0, 1, 0.02);
    guiMidi.sliderPanel.sliders[3].setRange(0, 127, 1);
    guiMidi.sliderPanel.sliders[4].setRange(0, 127, 1);

    guiMidi.sliderPanel.sliders[5].setRange(-8, 8, 1);
    guiMidi.sliderPanel.sliders[6].setRange(-12, 12, 1);
    
    guiMidi.sliderPanel.sliders[7].setRange(0, 32, 1);
    
    guiMidi.sliderPanel.sliders[8].setRange(0, 1, 0.05);
    guiMidi.sliderPanel.sliders[9].setRange(0, 1, 0.05);

    guiMidi.sliderPanel.sliders[0].setValue(params.smoothTime.dBShort);
    guiMidi.sliderPanel.sliders[1].setValue(params.smoothTime.dBLong);
    
    //guiMidi.sliderPanel.sliders[2].setValue(params.velocity.ratio);
    guiMidi.sliderPanel.sliders[3].setValue(params.velocity.min);
    guiMidi.sliderPanel.sliders[4].setValue(params.velocity.max);
    
    guiMidi.sliderPanel.sliders[5].setValue(params.shift.octave);
    guiMidi.sliderPanel.sliders[6].setValue(params.shift.semitone);
    
    guiMidi.sliderPanel.sliders[7].setValue(params.smoothTime.midi);
    
    guiMidi.sliderPanel.sliders[8].setValue(params.threshold.retrig);
    guiMidi.sliderPanel.sliders[9].setValue(params.threshold.retrigStop);
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ScribeAudioProcessorEditor::updateSpectrum()
{
    for (int i = 0; i < scribe.weights.size(); i++)
    {
        guiSpectrum.bars.weights[i] = scribe.fundamentalHistory[i];
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

void ScribeAudioProcessorEditor::updateMidi(bool send) 
{
    if (send) 
    {

    }
    
}

void ScribeAudioProcessorEditor::updateSettings() {}
