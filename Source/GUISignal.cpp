/*
  ==============================================================================

    GUISignal.cpp
    Created: 1 Oct 2020 2:51:44pm
    Author:  cjack

  ==============================================================================
*/

#include "GUISignal.h"


GuiSignal::GuiSignal(int signalSize, int bufferSize) : scope(signalSize), meter(bufferSize)
{
    addAndMakeVisible(meter);
    addAndMakeVisible(scope);
}

void GuiSignal::resized() 
{
    auto area = getLocalBounds();

    float pad = area.getHeight() * 0.05f;

    REMOVE_FROM_ALL_SIDES(area, pad);

    auto meterArea = area.removeFromTop(area.getHeight() * 0.5f);
    meterArea.removeFromBottom(pad * 0.5f);

    auto scopeArea = area;
    scopeArea.removeFromTop(pad * 0.5f);

    scope.setBounds(scopeArea);
    meter.setBounds(meterArea);




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
    Y = isnan(Y) ? 0 : Y;

    signalPath.startNewSubPath(X, Y);

    for (int i = 1; i < signalVec.size(); i++)
    {
        X += W;
        Y = (signalVec[i] + max) / (2 * max);
        Y = Y > 1 ? 1 : Y;
        Y = H * Y;
        Y = isnan(Y) ? 0 : Y;
        signalPath.lineTo(X, Y);
    }

    g.setColour(BOLD_BLACK_INK);
    g.strokePath(signalPath, stroke);

    g.drawRect(getLocalBounds(), 2);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SignalMeter::SignalMeter(int bufferSize) : dBBuffer(bufferSize, -60)
{
}



void SignalMeter::paint(juce::Graphics& g) 
{
    float mindB = -60.0f;

    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    auto area = getLocalBounds();
    auto text = area.removeFromLeft(area.getWidth() * 0.1);
    auto lines = area;

    float X = lines.getX();
    float Y = lines.getY();
    float W = lines.getWidth();
    float H = lines.getHeight();

    float section = 1.0f / 6.0f;

    g.setColour(FADE_BLACK_INK);
    text.removeFromTop(H * section * 0.5);
    g.drawText("-10", text.removeFromTop(H * section ), juce::Justification::centred, false);
    g.drawText("-20", text.removeFromTop(H * section ), juce::Justification::centred, false);
    g.drawText("-30", text.removeFromTop(H * section ), juce::Justification::centred, false);
    g.drawText("-40", text.removeFromTop(H * section ), juce::Justification::centred, false);
    g.drawText("-50", text.removeFromTop(H * section ), juce::Justification::centred, false);

    section = 1.0f / 6.0f;
#define METER_LINE(frac){ g.drawLine(X, H * frac, W, H* frac, 2);}

    METER_LINE(section * 1);
    METER_LINE(section * 2);
    METER_LINE(section * 3);
    METER_LINE(section * 4);
    METER_LINE(section * 5);

#undef METER_LINE

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    juce::Path dBPath;
    juce::PathStrokeType stroke(2.0f);

    dBPath.preallocateSpace(dBBuffer.size * 3);

    std::vector<float> signal = dBBuffer.toOrderedVec();

    X = 0;
    Y = 0;
    W = getWidth() / (float)dBBuffer.size;
    H = getHeight();

#define DB_Y(Y, val, dBrange) {\
    Y = val / dBrange;\
    Y = Y > 0 ? 0 : Y;\
    Y = Y < -1 ? -1 : Y;\
    Y = H - (H * (Y + 1));\
}

    DB_Y(Y, dBBuffer.currentValue(), -mindB);

    dBPath.startNewSubPath(X, Y);

    for (int i = 1; i < signal.size(); i++) 
    {
        X += W;
        DB_Y(Y, signal[i], -mindB);
        dBPath.lineTo(X, Y);

    }

    g.setColour(BOLD_BLACK_INK);
    g.strokePath(dBPath, stroke);

    g.drawRect(getLocalBounds(), 2);

#undef DB_Y
}



