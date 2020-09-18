
#include "GUI.h"
/*
All major sections may have some sub components used for organization
The below list is major components only

ctrl + f #. ClassName

0. Globals
1. GuiParams - parameters that power the note selection algorithm
2. GuiSpectrum - customized frequency spectrum using logic in DCT.cpp
3. GuiWindow - View into the window used to generate GuiSpectrum and amplitude data
3. GuiLog - print out of all useful info generated within the audioProcessBlock
4. GuiSettings - user settings/saving/loading etc.
5. GuiTabs - holds everything together
*/

//1. GuiParams ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//temporary macros to test things out

#define PARAM_ITER_SLIDER_RESIZE {\
    auto sliderArea = area.removeFromTop(getHeight() * 0.8f);\
    for (int i = 0; i < sliders.size(); i++)\
    {\
        sliders[i].setBounds(sliderArea.removeFromLeft(getWidth() * 0.1f));\
        labels[i].setBounds(area.removeFromLeft(getWidth() * 0.1f));\
    }\
}

#define PARAM_DESC_RESIZE desc.setBounds(area.removeFromLeft(getWidth() * 0.2f));

#define PARAM_ITER_SLIDER_INIT(style) {\
    for (int i = 0; i < sliders.size(); i++)\
    {\
        sliders[i].setSliderStyle(style);\
        sliders[i].setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);\
        sliders[i].setRange(ranges[i], intervals[i]);\
        sliders[i].setName(hoverTips[i]);\
    }\
}

#define PARAM_ITER_LABEL_INIT {\
    for (int i = 0; i < labels.size(); i++)\
    {\
        labels[i].setText(labelText[i], juce::NotificationType::dontSendNotification);\
        labels[i].setJustificationType(juce::Justification(36));\
    }\
}

#define PARAM_ITER_VISIBLE_ADD(vec) {\
    for (int i = 0; i < vec.size(); i++)\
    {\
        addAndMakeVisible(vec[i]);\
    }\
}

void ParamInfo::resized()
{
    auto area =  getLocalBounds();

    //center the status bar by removing padding
    float wpad = getWidth() * 0.4f;
    float hpad = getHeight() * 0.1f;

    area.removeFromLeft(wpad);
    area.removeFromRight(wpad);
    area.removeFromTop(hpad);
    area.removeFromBottom(hpad);

    status.setBounds(area);
}

ParamNoteID::ParamNoteID()
{
    addAndMakeVisible(desc);

    initText(desc, "Note Idenification", 36);


    PARAM_ITER_VISIBLE_ADD(sliders);
    PARAM_ITER_VISIBLE_ADD(labels);

    PARAM_ITER_SLIDER_INIT(juce::Slider::SliderStyle::LinearVertical);
    PARAM_ITER_LABEL_INIT
}

void ParamNoteID::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    PARAM_DESC_RESIZE;

    PARAM_ITER_SLIDER_RESIZE;

}

ParamAmplitude::ParamAmplitude()
{
    addAndMakeVisible(desc);
    initText(desc, "Amplitude Thresholds", 36);
    
    PARAM_ITER_VISIBLE_ADD(sliders);
    PARAM_ITER_VISIBLE_ADD(labels);
    
    PARAM_ITER_SLIDER_INIT(juce::Slider::SliderStyle::LinearVertical);
    PARAM_ITER_LABEL_INIT

}

void ParamAmplitude::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    PARAM_DESC_RESIZE;

    PARAM_ITER_SLIDER_RESIZE;

}

ParamShift::ParamShift()
{
    addAndMakeVisible(desc);


    initText(desc, "Midi Note Shift", 36);
    
    PARAM_ITER_VISIBLE_ADD(sliders);
    PARAM_ITER_VISIBLE_ADD(labels);

    PARAM_ITER_SLIDER_INIT(juce::Slider::SliderStyle::LinearVertical);
    PARAM_ITER_LABEL_INIT



}

void ParamShift::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    PARAM_DESC_RESIZE;

    PARAM_ITER_SLIDER_RESIZE;

}

ParamVelocity::ParamVelocity()
{
    addAndMakeVisible(desc);

    initText(desc, "Velocity Mapping", 36);

    PARAM_ITER_VISIBLE_ADD(sliders);
    PARAM_ITER_VISIBLE_ADD(labels);

    PARAM_ITER_SLIDER_INIT(juce::Slider::SliderStyle::TwoValueVertical);
    PARAM_ITER_LABEL_INIT
}

void ParamVelocity::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    PARAM_DESC_RESIZE;

    PARAM_ITER_SLIDER_RESIZE;
}


GuiParams::GuiParams()
{
    setSize(REFX, REFY);

    addAndMakeVisible(status);
    initText(status, defaultStatus, 36);
    
    addAndMakeVisible(noteID);
    addAndMakeVisible(amplitude);
    addAndMakeVisible(shift);
    addAndMakeVisible(velocity);

    addSliderListeners(noteID.sliders);
    addSliderListeners(amplitude.sliders);
    addSliderListeners(shift.sliders);
    addSliderListeners(velocity.sliders);
}

void GuiParams::addSliderListeners(std::vector<juce::Slider>& sliders)
{
    for (int i = 0; i < sliders.size(); i++)
    {
        sliders[i].addListener((juce::Slider::Listener*)this);
    }
}

void GuiParams::mouseEnter(const juce::MouseEvent& event) 
{
    std::string(to_string((juce::Slider)event.eventComponent.getValue()));
}

void GuiParams::mouseExit(const juce::MouseEvent& event) 
{
    status.setText(defaultStatus, juce::NotificationType::dontSendNotification);
}

void GuiParams::resized()
{
    auto area = getLocalBounds();
    float infoH = getHeight() * 0.08f;
    float sectionH = getHeight() * 0.22f;

    status.setBounds(area.removeFromTop(infoH));

    for (int i = 0; i < mainSections.size(); i++)
    {
        mainSections[i]->setBounds(area.removeFromTop(sectionH));
    }
    
}



//2. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiSpectrum::GuiSpectrum() {}
void GuiSpectrum::resized() {}

//3. GuiWindow ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiWindow::GuiWindow() {}
void GuiWindow::resized() {}

//4. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiLog::GuiLog() {}
void GuiLog::resized() {}

//5. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiSettings::GuiSettings() {}
void GuiSettings::resized() {}

//6. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
