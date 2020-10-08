/*
  ==============================================================================

    GUIMidi.cpp
    Created: 1 Oct 2020 2:52:04pm
    Author:  cjack

  ==============================================================================
*/

#include "GUIMidi.h"


GuiMidi::GuiMidi()
{
    addAndMakeVisible(sliderPanel);
}

void GuiMidi::resized() 
{
    auto area = getLocalBounds();
    float pad = area.getWidth() * 0.05f;

    REMOVE_FROM_ALL_SIDES(area, pad);

    auto panelArea = area.removeFromLeft(area.getWidth() * 0.4f);

    panelArea.removeFromRight(pad);

    sliderPanel.setBounds(panelArea);
    
}

#define SHOW_DESC_THEN_VAL(desc, val, currentArea) {\
g.drawText(desc, currentArea.removeFromLeft(currentArea.getWidth() * 0.5f), juce::Justification::centred);\
g.drawText(val, currentArea, juce::Justification::centred);\
}
void GuiMidi::paint(juce:: Graphics& g) 
{
    auto area = getLocalBounds();

    area.removeFromLeft(sliderPanel.getWidth());
    area.removeFromLeft(area.getWidth() * 0.05f);
    area.removeFromRight(area.getWidth() * 0.5f);

    int H = area.getHeight() / 4;
    
    auto midiOnArea = area.removeFromTop(H);
    auto velOnArea = area.removeFromTop(H);
    
    auto retrigArea = area.removeFromTop(H);

    SHOW_DESC_THEN_VAL("Midi On: ", juce::String(midiOn), midiOnArea);
    SHOW_DESC_THEN_VAL("Vel On: ", juce::String(velOn), velOnArea);

    SHOW_DESC_THEN_VAL("Retrigger %: ", juce::String(retrigger), retrigArea);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MidiSliders::MidiSliders() : sliders(10), labels(10)
{

    APPLY_FUNC_TO_ELEM(addAndMakeVisible, sliders);
    APPLY_FUNC_TO_ELEM(addAndMakeVisible, labels);
}

void MidiSliders::resized()
{
    auto area = getLocalBounds();

    float H = area.getHeight() / (float)sliders.size();

    for (int i = 0; i < sliders.size(); i++)
    {
        auto rect = area.removeFromTop(H);
        labels[i].setBounds(rect.removeFromLeft(rect.getWidth() * 0.50f));
        sliders[i].setBounds(rect);
        sliders[i].setTextBoxStyle(
            juce::Slider::TextEntryBoxPosition::TextBoxBelow,
            true,
            rect.getWidth(),
            rect.getHeight() * 0.50f);
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