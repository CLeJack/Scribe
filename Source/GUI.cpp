
#include "GUI.h"
/*
All major sections may have some sub components used for organization
The below list is major components only

ctrl + f #. ClassName

1. GuiParams - parameters that power the note selection algorithm
2. GuiSpectrum - customized frequency spectrum using logic in DCT.cpp
3. GuiLog - print out of all useful info generated within the audioProcessBlock
4. GuiSettings - user settings/saving/loading etc.
5. GuiTabs - holds everything together
*/

//1. GuiParams ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ParamInfo::ParamInfo()
{
    addAndMakVisible(status);
    status.setText("Status");
    status.setJustification(juce::Justication(36))

}

void ParamInfo::resized() override
{
    //center the status bar by removing padding
    float wpad = getWidth()*0.5f - status.getWidth()*0.5f;
    float hpad = getHeight()*0.5f - status.getHeight()*0.5f;

    auto area =  getLocalBounds();

    area.removeFromLeft(wpad);
    area.removeFromRight(wpad);
    area.removeFromTop(hpad);
    area.removeFromBottom(hpad);

    status.setBounds(area);
}

ParamSelection::ParamSelection()
{
    

}

ParamAmplitude::ParamAmplitude(){}
ParamShift::ParamShift(){}
ParamVelocity::ParamVelocity(){}

GuiParams::GuiParams(){}



//2. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//3. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//4. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//5. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
