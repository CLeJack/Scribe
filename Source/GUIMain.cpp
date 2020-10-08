/*
  ==============================================================================

    GUIMain.cpp
    Created: 1 Oct 2020 2:52:15pm
    Author:  cjack

  ==============================================================================
*/

#include "GUIMain.h"

void GuiMain::paint(juce::Graphics& g) 
{
    float srcY = getHeight() * 1.0f / 3.0f;
    float srcX = getWidth() * 0.25f;
    float srcW = 2*srcX;
    float srcH = srcY;

    auto rect = juce::Rectangle<float>(srcX, srcY, srcW, srcH);

    g.drawImage(logo, rect);

}