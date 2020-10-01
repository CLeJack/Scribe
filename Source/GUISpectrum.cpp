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

    float X = area.getX();
    float Y = area.getY();
    float H = area.getHeight();
    float W = area.getWidth();

    pad = std::min(H, W) * 0.05f;
    
    auto sliderArea = area.removeFromLeft(pad * 3);
    
    area.removeFromLeft(pad);

    auto displayArea = area.removeFromLeft(pad * 12);

    area.removeFromLeft(pad);

    auto dBArea = area; //remaining 15%

    //set all bounds~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sliderPanel.setBounds(sliderArea);
    dB.setBounds(dBArea);


    background.setBounds (displayArea);

    pad = displayArea.getWidth() * 0.05f;

    REMOVE_FROM_ALL_SIDES(displayArea, pad);

    notes.setBounds(displayArea.removeFromBottom(displayArea.getHeight() * 0.1f));

    thresholds.setBounds (displayArea);
    bars.setBounds       (displayArea);
    

    

}