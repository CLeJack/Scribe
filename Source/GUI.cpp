
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

//temporary expansion to test this out

ParamInfo::ParamInfo()
{
    addAndMakeVisible(status);
    initText(status, "Status", 36);

}

void ParamInfo::resized()
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
    resizeH(area, (int)getHeight() * 0.05f);

    float descWidth = getWidth() * 0.2f;
    float sliderWidth = getWidth() * 0.1f;

    desc.setBounds(area.removeFromLeft(descWidth));

    for(auto s = sliders.begin(); s != sliders.end(); s++)
    {
        *s.setBounds(area.removeFromLeft(sliderWidth));
    }

}

ParamAmplitude::ParamAmplitude()
{
    addAndMakeVisible(desc);
    addAndMakeVisible(noise);
    addAndMakeVisible(release);

    initText(desc, "Amplitude Thresholds", 36);
    
    noise.setName("Note Trigger Threshold: ");
    release.setName("Note Release Threshold: ");

}

ParamAmplitude::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    float descWidth = getWidth() * 0.2f;
    float sliderWidth = getWidth() * 0.1f;

    desc.setBounds(area.removeFromLeft(descWidth));

    for(auto s = sliders.begin(); s != sliders.end(); s++)
    {
        *s.setBounds(area.removeFromLeft(sliderWidth));
    }

}

ParamShift::ParamShift()
{
    addAndMakeVisible(desc);
    addAndMakeVisible(octave);
    addAndMakeVisible(semitone);

    initText(desc, "Midi Note Shift", 36);
    octave.setName("Octave Shift: ");
    semitone.setName("Semitone Shift: ");
}

ParamShift::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    float descWidth = getWidth() * 0.2f;
    float sliderWidth = getWidth() * 0.1f;

    desc.setBounds(area.removeFromLeft(descWidth));

    for(auto s = sliders.begin(); s != sliders.end(); s++)
    {
        *s.setBounds(area.removeFromLeft(sliderWidth));
    }

}

ParamVelocity::ParamVelocity()
{
    addAndMakeVisible(desc);
    addAndMakeVisible(decibel);
    addAndMakeVisible(velocity);
}

ParamVelocity::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    float descWidth = getWidth() * 0.2f;
    float sliderWidth = getWidth() * 0.1f;

    desc.setBounds(area.removeFromLeft(descWidth));

    for(auto s = sliders.begin(); s != sliders.end(); s++)
    {
        *s.setBounds(area.removeFromLeft(sliderWidth));
    }
}

GuiParams::GuiParams()
{
    addAndMakeVisible(info);
    addAndMakeVisible(noteID);
    addAndMakeVisible(amplitude);
    addAndMakeVisible(shift);
    addAndMakeVisible(velocity);
}

GuiParams::resized()
{
    auto area = getLocalBounds();
    float infoH = getHeight() * 0.8f;
    float sectionH = getHeight() * 0.23f;

    info.setBounds(area.removeFromTop(infoH));

    for(auto s = mainSections.begin(); s != mainSections.end(); s++)
    {
        *s.setBounds(area.removeFromTop(sectionH));
    }
    
}



//2. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//3. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//4. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//5. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
