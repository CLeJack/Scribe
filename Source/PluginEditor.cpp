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
        A::Threshold::weight = slider->getValue();
    }
    else if (slider == &spectrum[1])
    {
        A::Scale::weight = slider->getValue();
    }
    else if (slider == &spectrum[2])
    {
        A::Range::lowNote = slider->getValue();
    }
    else if (slider == &spectrum[3])
    {
        A::Threshold::ratio = slider->getValue();
    }

    else if (slider == &signal[0])
    {
        A::Threshold::noise = slider->getValue();
    }
    else if (slider == &signal[1])
    {
        A::Scale::noise = slider->getValue();
    }
    else if (slider == &signal[2])
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