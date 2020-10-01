/*
  ==============================================================================

    GUISpectrum.cpp
    Created: 1 Oct 2020 2:51:19pm
    Author:  cjack

  ==============================================================================
*/

#include "GUISpectrum.h"


GuiSpectrum::GuiSpectrum(int barCount, int octaveSize) : background(barCount, octaveSize), bars(barCount), notes(barCount, octaveSize)
{
}

void GuiSpectrum::resized() 
{
    auto area = getLocalBounds();
    
    float X = 0;
    float Y = 0;
    float H = area.getHeight();
    float W = area.getWidth();

    float pad = std::min(H, W) * 0.1f;

    REMOVE_FROM_ALL_SIDES(area, pad);

    X = area.getX();
    Y = area.getY();
    H = area.getHeight();
    W = area.getWidth();

    pad = std::min(H, W) * 0.05f;
    
    auto sliderArea = area.removeFromLeft(pad * 3);
    
    area.removeFromLeft(pad);

    auto displayArea = area.removeFromLeft(pad * 12);

    area.removeFromLeft(pad);

    auto dBArea = area; //remaining 15%

    //set all bounds~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sliderPanel.setBounds(sliderArea);
    meter.setBounds(dBArea);


    background.setBounds (displayArea);

    pad = displayArea.getWidth() * 0.05f;

    REMOVE_FROM_ALL_SIDES(displayArea, pad);

    notes.setBounds(displayArea.removeFromBottom(displayArea.getHeight() * 0.1f));

    thresholds.setBounds (displayArea);
    bars.setBounds       (displayArea);
    

    

}

SpectrumSliders::SpectrumSliders() 
{
    labels[0].setText("Noise Floor", juce::NotificationType::dontSendNotification);
    labels[1].setText("Noise Slope", juce::NotificationType::dontSendNotification);
    labels[2].setText("Low Note", juce::NotificationType::dontSendNotification);
    labels[3].setText("Octave Ratio", juce::NotificationType::dontSendNotification);

    sliders[0].setRange(0, 100, 1);
    sliders[1].setRange(-20, 0, 1);
    sliders[2].setRange(12, 28, 1);
    sliders[3].setRange(0, 10, 1);

}

void SpectrumSliders::resized() 
{
    auto area = getLocalBounds();

    float H = area.getHeight() * 0.25f;

    for (int i = 0; i < sliders.size(); i++) 
    {
        auto rect = area.removeFromTop(H);
        labels[i].setBounds(rect.removeFromBottom(rect.getHeight() * 0.15f));
        sliders[i].setBounds(area);
    }

}

SpectrumBackground::SpectrumBackground(float barCount, float octaveSize) : barCount(barCount), octaveSize(octaveSize) 
{
}

void SpectrumBackground::paint(juce::Graphics& g) 
{
    
    auto area = getLocalBounds();
    int segments = barCount / octaveSize;
    float W = area.getWidth();
    float w = W / segments;
    float X = getX();
    float Y = getY();
    float H = getHeight();

    for (int i = 0; i < segments; i++) 
    {
        g.setColour(i % 2 == 0 ? PAPER_DARK : PAPER);
        g.fillRect(X, Y, w, H);
    }

    g.setColour(BOLD_BLACK_INK);
    g.drawRect(getLocalBounds(), 2);
}


#define SET_SPECTRUM_THRESHOLDS(color, index){\
g.setColour(color);\
Y = floor - relativeHeights[index] * H;\
g.drawLine(X, Y, W, Y); \
}
void SpectrumThresholds::paint(juce::Graphics& g) 
{
    float Y = getY();
    float floor = getBottom();
    float H = getHeight();
    float X = getX();
    float W = getRight();


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
    float X = getX();
    float W = getWidth() / weights.size();
    float right = getRight();
    float floor = getBottom();
    float H = getHeight();

    int maxInd = maxArg(weights);

    g.setColour(BOLD_BLACK_INK);

    for (int i = 0; i < weights.size(); i++) 
    {
        Y = H * weights[i];

        if (i == maxInd) 
        {
            g.setColour(BOLD_GREEN_INK);
            g.fillRect(X, floor - Y, W, Y);
            g.setColour(BOLD_BLACK_INK);
        }


        g.drawRect(X,floor - Y, W, Y);

        X += W;
    }

    g.setColour(BOLD_GREEN_INK);

    Y = H * weights[maxInd];
    g.drawLine(getX(), floor - Y, right, floor - Y, 2.0f);
}

SpectrumNotes::SpectrumNotes(float barCount, float octaveSize) : panel(barCount)
{
    for(int i = 0; i < panel.size(); i++)
    {
        panel[i] = letters[i % (int)octaveSize];
    }
}


void SpectrumdB::paint(juce::Graphics& g) 
{
    dB = dB < mindB ? mindB : dB;
    float pct = std::abs((mindB - dB) / mindB);
    auto area = getLocalBounds();

    g.setColour(FADE_BLACK_INK);

    g.fillRect(area.removeFromBottom(area.getHeight() * pct));

    g.setColour(BOLD_BLACK_INK);

    g.drawRect(getLocalBounds());
}
