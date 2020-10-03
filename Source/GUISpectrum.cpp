/*
  ==============================================================================

    GUISpectrum.cpp
    Created: 1 Oct 2020 2:51:19pm
    Author:  cjack

  ==============================================================================
*/

#include "GUISpectrum.h"


GuiSpectrum::GuiSpectrum(int barCount, int octaveSize) : 
    background(barCount, octaveSize), bars(barCount), notes(barCount, octaveSize)
{
    addAndMakeVisible(background);
    addAndMakeVisible(sliderPanel);
    addAndMakeVisible(thresholds);
    addAndMakeVisible(notes);
    addAndMakeVisible(bars);
}

void GuiSpectrum::resized() 
{
    auto area = getLocalBounds();
    
    float X = 0;
    float Y = 0;
    float H = area.getHeight();
    float W = area.getWidth();

    float pad = area.getHeight() * 0.05f;

    REMOVE_FROM_ALL_SIDES(area, pad);

    X = area.getX();
    Y = area.getY();
    H = area.getHeight();
    W = area.getWidth();


    auto sliderArea = area.removeFromLeft(getWidth() * 0.2f);
    area.removeFromLeft(pad);

    auto displayArea = area;


    //set all bounds~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sliderPanel.setBounds(sliderArea);
    

    background.setBounds (displayArea);

    notes.setBounds(displayArea.removeFromBottom(displayArea.getHeight() * 0.05f));

    thresholds.setBounds (displayArea);
    bars.setBounds       (displayArea);
    

    

}

SpectrumSliders::SpectrumSliders() : sliders(6), labels(6)
{

    APPLY_FUNC_TO_ELEM(addAndMakeVisible, sliders);
    APPLY_FUNC_TO_ELEM(addAndMakeVisible, labels);

}

void SpectrumSliders::resized() 
{
    auto area = getLocalBounds();

    float H = area.getHeight() / (float) sliders.size();

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

SpectrumBackground::SpectrumBackground(float barCount, float octaveSize) : barCount(barCount), octaveSize(octaveSize) 
{
}

void SpectrumBackground::paint(juce::Graphics& g) 
{

    


    int segments = barCount / octaveSize;
    float W = getWidth();
    float w = W / segments;
    float X = 0;
    float Y = 0;
    float H = getHeight();

    for (int i = 0; i < segments; i++) 
    {
        g.setColour(i % 2 == 0 ? PAPER_DARK : PAPER);
        g.fillRect(X, Y, w, H);
        
        X += w;
    }
    
}


SpectrumThresholds::SpectrumThresholds() : relativeHeights(4, 0) 
{
    for (int i = 0; i < relativeHeights.size(); i++) 
    {
        relativeHeights[i] = (i + 1) * .05;
    }
}

#define SET_SPECTRUM_THRESHOLDS(color, index){\
g.setColour(color);\
Y = H - relativeHeights[index] * H;\
g.drawLine(X, Y, W, Y, 2.0f); \
}
void SpectrumThresholds::paint(juce::Graphics& g) 
{
    float Y = 0;
    float H = getHeight();
    float X = 0;
    float W = getWidth();


    SET_SPECTRUM_THRESHOLDS(MARKER0, 0);
    SET_SPECTRUM_THRESHOLDS(MARKER1, 1);
    SET_SPECTRUM_THRESHOLDS(MARKER2, 2);
    SET_SPECTRUM_THRESHOLDS(MARKER3, 3);

}


SpectrumBars::SpectrumBars(float barCount) : weights(barCount) 
{
}

void SpectrumBars::paint(juce::Graphics& g) 
{
    float Y = 0;
    float X = 0;
    float W = getWidth() / (float)weights.size();
    float right = X+W;
    float H = getHeight();

    int maxInd = maxArg(weights);

    g.setColour(BOLD_BLACK_INK);

    for (int i = 0; i < weights.size(); i++) 
    {
        Y = H * weights[i];

        if (i == maxInd) 
        {
            g.setColour(BOLD_GREEN_INK);
            g.fillRect(X, H - Y, W, Y);
            g.setColour(BOLD_BLACK_INK);
        }


        g.drawRect(X,H - Y, W, Y, 2.0f);

        X += W;
    }

    g.setColour(BOLD_GREEN_INK);

    Y = H * weights[maxInd];
    g.drawLine(0, H - Y, getWidth(), H - Y, 2.0f);

    g.setColour(BOLD_BLACK_INK);
    g.drawRect(getLocalBounds(), 2.0f);
}

SpectrumNotes::SpectrumNotes(float barCount, float octaveSize) : keys(barCount)
{
    for(int i = 0; i < keys.size(); i++)
    {
        keys[i] = keyReference[i % (int)octaveSize];
    }
}



void SpectrumNotes::paint(juce::Graphics& g) 
{
    float X = 0;
    float Y = 0;
    float W = getWidth() / (float)keys.size();
    float H = getHeight();
    for (int i = 0; i < keys.size(); i++) 
    {
        g.setColour(keys[i] == 0 ? PAPER_LIGHT : FADE_BLACK_INK);
        g.fillRect(X, Y, W, H);
        X += W;
    }
}
