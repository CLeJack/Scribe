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
    guiSpectrum(Scribe::frequencies.size(), Scribe::Tuning::octaveSize),
    guiSignal(Scribe::DownSample::historySamples, 60, 4)
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

    namespace A = AudioParams;
    if (slider == &spectrum[0]) 
    {
        A::Threshold::weight0 = slider->getValue();
    }
    else if (slider == &spectrum[1])
    {
        A::Threshold::weight1 = slider->getValue();
    }
    else if (slider == &spectrum[2])
    {
        A::Threshold::weight2 = slider->getValue();
    }
    else if (slider == &spectrum[3])
    {
        A::Threshold::weight3 = slider->getValue();
    }
    else if (slider == &spectrum[4])
    {
        A::Range::lowNote = slider->getValue();
    }
    else if (slider == &spectrum[5])
    {
        A::Threshold::ratio = slider->getValue();
    }

    else if (slider == &signal[0])
    {
        A::Threshold::noise0 = slider->getValue();
    }
    else if (slider == &signal[1])
    {
        A::Threshold::noise1 = slider->getValue();
    }
    else if (slider == &signal[2])
    {
        A::Threshold::noise2 = slider->getValue();
    }
    else if (slider == &signal[3])
    {
        A::Threshold::noise3 = slider->getValue();
    }
    else if (slider == &signal[4])
    {
        A::SmoothTime::dB = slider->getValue();
    }

    else if (slider == &midi[0])
    {
        A::SmoothTime::amp = slider->getValue();
    }
    else if (slider == &midi[1])
    {
        A::Angle::amp = slider->getValue();
    }

    else if (slider == &midi[2])
    {
        A::Velocity::min = slider->getValue();
    }

    else if (slider == &midi[3])
    {
        A::Velocity::max = slider->getValue();
    }

    else if (slider == &midi[4])
    {
        A::Shift::octave = slider->getValue();
    }

    else if (slider == &midi[5])
    {
        A::Shift::semitone = slider->getValue();
    }


}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ScribeAudioProcessorEditor::setSliders()
{
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

    guiSpectrum.sliderPanel.sliders[0].setValue(AudioParams::Threshold::weight0);
    guiSpectrum.sliderPanel.sliders[1].setValue(AudioParams::Threshold::weight1);
    guiSpectrum.sliderPanel.sliders[2].setValue(AudioParams::Threshold::weight2);
    guiSpectrum.sliderPanel.sliders[3].setValue(AudioParams::Threshold::weight3);
    guiSpectrum.sliderPanel.sliders[4].setValue(AudioParams::Range::lowNote);
    guiSpectrum.sliderPanel.sliders[5].setValue(AudioParams::Threshold::ratio);



    guiSignal.sliderPanel.labels[0].setText("Noise 0", juce::NotificationType::dontSendNotification);
    guiSignal.sliderPanel.labels[1].setText("Noise 1", juce::NotificationType::dontSendNotification);
    guiSignal.sliderPanel.labels[2].setText("Noise 2", juce::NotificationType::dontSendNotification);
    guiSignal.sliderPanel.labels[3].setText("Noise 3", juce::NotificationType::dontSendNotification);
    guiSignal.sliderPanel.labels[4].setText("dB Delay ", juce::NotificationType::dontSendNotification);

    guiSignal.sliderPanel.sliders[0].setRange(-60, 0, 1);
    guiSignal.sliderPanel.sliders[1].setRange(-60, 0, 1);
    guiSignal.sliderPanel.sliders[2].setRange(-60, 0, 1);
    guiSignal.sliderPanel.sliders[3].setRange(-60, 0, 1);
    guiSignal.sliderPanel.sliders[4].setRange(0, 32, 1);

    guiSignal.sliderPanel.sliders[0].setValue(AudioParams::Threshold::noise0);
    guiSignal.sliderPanel.sliders[1].setValue(AudioParams::Threshold::noise1);
    guiSignal.sliderPanel.sliders[2].setValue(AudioParams::Threshold::noise2);
    guiSignal.sliderPanel.sliders[3].setValue(AudioParams::Threshold::noise3);
    guiSignal.sliderPanel.sliders[4].setValue(AudioParams::SmoothTime::dB);



    guiMidi.sliderPanel.labels[0].setText("Amp Delay", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[1].setText("Max Vel. Angle", juce::NotificationType::dontSendNotification);

    guiMidi.sliderPanel.labels[2].setText("Min Vel.", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[3].setText("Max Vel.", juce::NotificationType::dontSendNotification);

    guiMidi.sliderPanel.labels[4].setText("Oct. Shift", juce::NotificationType::dontSendNotification);
    guiMidi.sliderPanel.labels[5].setText("Semi Shift", juce::NotificationType::dontSendNotification);

    guiMidi.sliderPanel.labels[6].setText("Midi Delay", juce::NotificationType::dontSendNotification);


    guiMidi.sliderPanel.sliders[0].setRange(0, 32, 1);
    guiMidi.sliderPanel.sliders[1].setRange(45, 90, 1);
    guiMidi.sliderPanel.sliders[2].setRange(0, 127, 1);
    guiMidi.sliderPanel.sliders[3].setRange(0, 127, 1);
    guiMidi.sliderPanel.sliders[4].setRange(-8, 8, 1);
    guiMidi.sliderPanel.sliders[5].setRange(-12, 12, 1);
    guiMidi.sliderPanel.sliders[6].setRange(0, 32, 1);

    guiMidi.sliderPanel.sliders[0].setValue(AudioParams::SmoothTime::amp);
    guiMidi.sliderPanel.sliders[1].setValue(AudioParams::Angle::amp);
    guiMidi.sliderPanel.sliders[2].setValue(AudioParams::Velocity::min);
    guiMidi.sliderPanel.sliders[3].setValue(AudioParams::Velocity::max);
    guiMidi.sliderPanel.sliders[4].setValue(AudioParams::Shift::octave);
    guiMidi.sliderPanel.sliders[5].setValue(AudioParams::Shift::semitone);
    guiMidi.sliderPanel.sliders[6].setValue(AudioParams::SmoothTime::midi);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ScribeAudioProcessorEditor::updateSpectrum()
{
    for (int i = 0; i < Scribe::weights.size(); i++)
    {
        guiSpectrum.bars.weights[i] = Scribe::weights[i];
    }

    guiSpectrum.thresholds.relativeHeights[0] = AudioParams::Threshold::weight0;
    guiSpectrum.thresholds.relativeHeights[1] = AudioParams::Threshold::weight1;
    guiSpectrum.thresholds.relativeHeights[2] = AudioParams::Threshold::weight2;
    guiSpectrum.thresholds.relativeHeights[3] = AudioParams::Threshold::weight3;
}

void ScribeAudioProcessorEditor::updateSignal()
{
    for (int i = 0; i < Scribe::historyDS.size(); i++)
    {
        guiSignal.scope.signalVec[i] = Scribe::historyDS[i];
    }

    guiSignal.meter.dBBuffer.push(Calculations::Amp::dB);

    guiSignal.thresholds.relativeHeights[0] = AudioParams::Threshold::noise0;
    guiSignal.thresholds.relativeHeights[1] = AudioParams::Threshold::noise1;
    guiSignal.thresholds.relativeHeights[2] = AudioParams::Threshold::noise2;
    guiSignal.thresholds.relativeHeights[3] = AudioParams::Threshold::noise3;
}

void ScribeAudioProcessorEditor::updateMidi() {}

void ScribeAudioProcessorEditor::updateSettings() {}
