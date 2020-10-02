/*
  ==============================================================================

    GUISignal.cpp
    Created: 1 Oct 2020 2:51:44pm
    Author:  cjack

  ==============================================================================
*/

#include "GUISignal.h"


GuiSignal::GuiSignal(int signalSize, int bufferSize, int thresholdSize) : scope(signalSize), meter(bufferSize, thresholdSize)
{
    addAndMakeVisible(sliderPanel);
    addAndMakeVisible(meter);
    addAndMakeVisible(scope);
}

void GuiSignal::resized() 
{
    auto area = getLocalBounds();

    float pad = area.getHeight() * 0.05f;

    REMOVE_FROM_ALL_SIDES(area, pad);

    auto panelArea = area.removeFromLeft(area.getWidth() * 0.3f);
    
    area.removeFromLeft(pad);

    auto meterArea = area.removeFromTop(area.getHeight() * 0.5f);
    meterArea.removeFromBottom(pad * 0.5f);

    auto scopeArea = area;
    scopeArea.removeFromTop(pad * 0.5f);


    sliderPanel.setBounds(panelArea);
    scope.setBounds(scopeArea);
    meter.setBounds(meterArea);



}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SignalSliders::SignalSliders() : sliders(4), labels(4)
{
    labels[0].setText("Noise Floor", juce::NotificationType::dontSendNotification);
    labels[1].setText("Noise Scale", juce::NotificationType::dontSendNotification);
    labels[2].setText("Amp Delay (ms)", juce::NotificationType::dontSendNotification);
    labels[3].setText("dB Delay (ms)", juce::NotificationType::dontSendNotification);

    sliders[0].setRange(0, 100, 1);
    sliders[1].setRange(0, 100, 1);
    sliders[2].setRange(0, 25, 1);
    sliders[3].setRange(0, 25, 1);

    APPLY_FUNC_TO_ELEM(addAndMakeVisible, sliders);
    APPLY_FUNC_TO_ELEM(addAndMakeVisible, labels);
}

void SignalSliders::resized()
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
SignalScope::SignalScope(int signalSize) : signalVec(signalSize) 
{
}

void SignalScope::paint(juce::Graphics& g) 
{
    juce::Path signalPath;
    juce::PathStrokeType stroke(2.0f);

    signalPath.preallocateSpace(signalVec.size() * 3);

    float X = 0;
    float Y = 0;
    float W = getWidth() / (float)signalVec.size();
    float H = getHeight();
    float max = absMaxValue(signalVec);
    Y = (signalVec[0] + max) / (2 * max);
    Y = Y > 1 ? 1 : Y;
    Y = H - H * Y;

    signalPath.startNewSubPath(X, Y);

    for (int i = 1; i < signalVec.size(); i++)
    {
        X += W;
        Y = (signalVec[i] + max) / (2 * max);
        Y = Y > 1 ? 1 : Y;
        Y = H * Y;
        signalPath.lineTo(X, Y);
    }

    g.setColour(BOLD_BLACK_INK);
    g.strokePath(signalPath, stroke);

    g.drawRect(getLocalBounds(), 2);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SignalMeter::SignalMeter(int bufferSize, int thresholdSize) : dBBuffer(bufferSize, -60), thresholds(thresholdSize, 0)
{
}


#define DB_Y(Y, val, dBrange) {\
    Y = val / dBrange;\
    Y = Y > 0 ? 0 : Y;\
    Y = Y < -1 ? -1 : Y;\
    Y = H - (H * (Y + 1));\
}
void SignalMeter::paint(juce::Graphics& g) 
{
    juce::Path dBPath;
    juce::PathStrokeType stroke(2.0f);

    dBPath.preallocateSpace(dBBuffer.size * 3);

    std::vector<float> signal = dBBuffer.toOrderedVec();

    float X = 0;
    float Y = 0;
    float W = getWidth() / (float)dBBuffer.size;
    float H = getHeight();

    DB_Y(Y, dBBuffer.currentValue(), 60.0f);

    dBPath.startNewSubPath(X, Y);

    for (int i = 1; i < signal.size(); i++) 
    {
        X += W;
        DB_Y(Y, signal[i], 60.0f);
        dBPath.lineTo(X, Y);

    }

    g.setColour(BOLD_BLACK_INK);
    g.strokePath(dBPath, stroke);

    g.drawRect(getLocalBounds(), 2);

}



