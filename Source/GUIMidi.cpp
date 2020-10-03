/*
  ==============================================================================

    GUIMidi.cpp
    Created: 1 Oct 2020 2:52:04pm
    Author:  cjack

  ==============================================================================
*/

#include "GUIMidi.h"


GuiMidi::GuiMidi() : angle(45, 90, 5), dB(-60, 0, 4)
{
    addAndMakeVisible(sliderPanel);
    addAndMakeVisible(on);
    addAndMakeVisible(off);
    addAndMakeVisible(angle);
}

void GuiMidi::resized() 
{
    auto area = getLocalBounds();
    float pad = area.getWidth() * 0.05f;

    REMOVE_FROM_ALL_SIDES(area, pad);

    auto panelArea = area.removeFromLeft(area.getWidth() * 0.3f);
    
    auto angleArea = area.removeFromTop(area.getHeight() * 0.5f);

    auto dBArea = angleArea.removeFromRight(angleArea.getWidth() * 0.5f);

    auto onArea = area.removeFromLeft(area.getWidth() * 0.5f);

    auto offArea = area;

    panelArea.removeFromRight(pad);

    angleArea.removeFromLeft(angleArea.getWidth() * 1.0f / 3.0f);
    angleArea.removeFromRight(angleArea.getWidth() * 0.5f);

    dBArea.removeFromLeft(dBArea.getWidth() * 1.0f / 3.0f);
    dBArea.removeFromRight(dBArea.getWidth() * 0.5f);

    REMOVE_FROM_ALL_SIDES(onArea, pad);
    REMOVE_FROM_ALL_SIDES(offArea, pad);

    sliderPanel.setBounds(panelArea);
    angle.setBounds(angleArea);
    dB.setBounds(dBArea);
    on.setBounds(onArea);
    off.setBounds(offArea);

    
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MidiSliders::MidiSliders() : sliders(6), labels(6)
{
    labels[0].setText("Amp Delay (ms)", juce::NotificationType::dontSendNotification);
    labels[1].setText("Max Vel. Angle", juce::NotificationType::dontSendNotification);
    
    labels[2].setText("Min Vel.", juce::NotificationType::dontSendNotification);
    labels[3].setText("Max Vel.", juce::NotificationType::dontSendNotification);
    
    labels[4].setText("Oct. Shift", juce::NotificationType::dontSendNotification);
    labels[5].setText("Semi Shift", juce::NotificationType::dontSendNotification);
    

    sliders[0].setRange(0, 25, 1);
    sliders[1].setRange(45, 90, 1);
    sliders[2].setRange(0, 127, 1);
    sliders[3].setRange(0, 127, 1);
    sliders[4].setRange(-8, 8, 1);
    sliders[5].setRange(-12, 12, 1);

    sliders[0].setValue(11);
    sliders[1].setValue(65);
    sliders[2].setValue(127);
    sliders[3].setValue(40);
    sliders[4].setValue(0);
    sliders[5].setValue(0);

    

    APPLY_FUNC_TO_ELEM(addAndMakeVisible, sliders);
    APPLY_FUNC_TO_ELEM(addAndMakeVisible, labels);
}

void MidiSliders::resized()
{
    auto area = getLocalBounds();

    float H = area.getHeight() * 0.25f;

    for (int i = 0; i < sliders.size(); i++)
    {
        auto rect = area.removeFromTop(H);
        labels[i].setBounds(rect.removeFromBottom(rect.getHeight() * 0.30f));
        sliders[i].setBounds(rect);
        sliders[i].setTextBoxStyle(
            juce::Slider::TextEntryBoxPosition::TextBoxLeft,
            true,
            rect.getWidth() * 0.30f,
            rect.getHeight() * 0.30f);
    }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MidiBar::MidiBar(float min, float max, float ticks) : min(min), max(max), ticks(ticks) 
{
}

void MidiBar::paint(juce::Graphics& g) 
{
    g.setColour(BOLD_BLACK_INK);
    g.drawRect(getLocalBounds(), 2);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MidiData::paint(juce::Graphics& g) 
{
    g.setColour(BOLD_BLACK_INK);
    g.drawRect(getLocalBounds(), 2);
}