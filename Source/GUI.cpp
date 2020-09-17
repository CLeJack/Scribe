
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
inline void initText(juce::DrawableText component, const char* text, int justification)
{
    component.setText(text);
    component.setJustification(jce::Justification(justification))
}

ParamInfo::ParamInfo()
{
    addAndMakeVisible(status);
    initText(status, "Status", 36);

}

void ParamInfo::resized() override
{
    auto area =  getLocalBounds();

    //center the status bar by removing padding
    float wpad = getWidth()*0.5f - status.getWidth()*0.5f;
    float hpad = getHeight()*0.5f - status.getHeight()*0.5f;

    area.removeFromLeft(wpad);
    area.removeFromRight(wpad);
    area.removeFromTop(hpad);
    area.removeFromBottom(hpad);

    status.setBounds(area);
}

ParamNoteID::ParamNoteID()
{
    addAndMakeVisible(desc);
    addAndMakeVisible(loOct);
    addAndMakeVisible(octStr);
    addAndMakeVisible(weight);
    addAndMakeVisible(retrigger);

    initText(desc, "Node Idenification", 36);

    loOct.setName("Lowest Octave: ");
    octStr.setName("Octave Strength Threshold: ");
    weight.setName("Note Activation Threshold: ");
    retrigger.setName("Note Retrigger: ");
}

ParamNoteID::resized()
{
    auto area = getLocalBounds();
    float padh = getHeight() * 0.05f;

    area.removeFromTop(padh);
    area.removeFromBottom(padh);

    float descWidth = getWidth() * 0.2f;
    float sliderWidth = getWidth()*0.1f;

    desc.setBounds(area.removeFromLeft(descWidth));

    loOct.setBounds(area.removeFromLeft(sliderWidth));
    octStr.setBounds(area.removeFromLeft(sliderWidth));
    weight.setBounds(area.removeFromLeft(sliderWidth));
    retrigger.setBounds(area.removeFromLeft(sliderWidth));

}

ParamAmplitude::ParamAmplitude()
{
    addAndMakeVisible(desc);
    addAndMakeVisible(noise);
    addAndMakeVisible(release);
}

ParamShift::ParamShift()
{
    addAndMakeVisible(desc);
    addAndMakeVisible(octave);
    addAndMakeVisible(semitone);
}

ParamVelocity::ParamVelocity()
{
    addAndMakeVisible(desc);
    addAndMakeVisible(decibel);
    addAndMakeVisible(velocity);
}

GuiParams::GuiParams()
{
    addAndMakeVisible(info);
    addAndMakeVisible(selection);
    addAndMakeVisible(amplitude);
    addAndMakeVisible(shift);
    addAndMakeVisible(velocity);
}



//2. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//3. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//4. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//5. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
