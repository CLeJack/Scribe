/*
  ==============================================================================

    GUIMain.cpp
    Created: 1 Oct 2020 2:52:15pm
    Author:  cjack

  ==============================================================================
*/

#include "GUIMain.h"

void GuiMainTab::paint(juce::Graphics& g) 
{
    float srcY = getHeight() * 1.0f / 3.0f;
    float srcX = getWidth() * 0.25f;
    float srcW = 2*srcX;
    float srcH = srcY;

    auto rect = juce::Rectangle<float>(srcX, srcY, srcW, srcH);

    g.drawImage(logo, rect);

}

GuiMainSlider::GuiMainSlider(const char* text, bool showText, double min, double max, double step) : 
    showText(showText)
{
    slider.setRange(min, max, step);
    this->text.setText(text, juce::NotificationType::dontSendNotification);
    this->text.setJustificationType(juce::Justification::centredTop);
    this->text.setBorderSize(juce::BorderSize<int>(0));

    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    //slider.setRotaryParameters(3.1415 * 225.0f / 180.0f, 3.1415 * 315.0f / 180.0f, true);

    addAndMakeVisible(this->text);
    addAndMakeVisible(slider);
}

void GuiMainSlider::resized()
{
    auto area = getLocalBounds();
    if (showText) 
    {
        auto sliderArea = area.removeFromTop(area.getHeight() * 0.75);

        auto textArea = area;

        slider.setBounds(sliderArea);
        text.setBounds(textArea);
    }
    else 
    {
        slider.setBounds(area);
    }
    
}

void GuiMainSlider::paint(juce::Graphics& g) 
{
    auto area = slider.getBoundsInParent();

    float padH = area.getHeight() * 0.333;

    area.removeFromTop(padH);
    area.removeFromBottom(padH);

    g.drawText(juce::String(slider.getValue(), 0, false), area, juce::Justification::centred);
}

GuiMainPanel::GuiMainPanel() :
    maxVel("Max Vel", true, 0, 127, 1),
    minVel("Min Vel", true, 0, 127, 1),
    maxdB("Max dB", true, -60, 0, 1),
    noise("Noise dB", true, -60, 0, 1),
    lowNote("Low Note", true, 12, 28, 1),
    octave("Octave", true, -8, 8, 1),
    semitone("Semitone", true, -12, 12, 1)
{
 

    midiLabel.setText("Midi", juce::NotificationType::dontSendNotification);
    midiLabel.setJustificationType(juce::Justification::centred);
    midiLabel.setBorderSize(juce::BorderSize<int>(0));

    panic.setButtonText("!");

    addAndMakeVisible(spectrumLabel);
    
    addAndMakeVisible(midiLabel);

    addAndMakeVisible(maxVel);
    addAndMakeVisible(minVel);
    
    addAndMakeVisible(maxdB);
    addAndMakeVisible(noise);
    
    addAndMakeVisible(lowNote);
    addAndMakeVisible(octave);
    addAndMakeVisible(semitone);


    addAndMakeVisible(panic);
}


void GuiMainPanel::resized() 
{
    auto area = getLocalBounds();

    float textH = area.getHeight() * 0.05;
    float sliderH = area.getHeight() * 0.9 * 0.2;


    auto sliderArea = area;

#define SET_AREA(sl, sliderArea, frac){\
sl.setBounds (sliderArea.removeFromLeft (sliderArea.getWidth() * frac));\
}

    midiLabel.setBounds(area.removeFromTop(textH));

    sliderArea = area.removeFromTop(sliderH);

    SET_AREA(maxVel, sliderArea, 0.333);
    SET_AREA(minVel, sliderArea, 0.5);

    sliderArea = area.removeFromTop(sliderH);

    SET_AREA(maxdB, sliderArea, 0.333);
    SET_AREA(noise, sliderArea, 0.5);


    sliderArea = area.removeFromTop(sliderH);

    SET_AREA(lowNote, sliderArea, 0.333);
    SET_AREA(octave, sliderArea, 0.5);
    SET_AREA(semitone, sliderArea, 1.0);

    area.removeFromLeft(area.getWidth() * 0.333);
    area.removeFromRight(area.getWidth() * 0.5);
    area.removeFromTop(area.getHeight() * 0.333);
    area.removeFromBottom(area.getHeight() * 0.5);
    panic.setBounds(area);

#undef SET_AREA
    
}