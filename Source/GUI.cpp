
#include "GUI.h"
/*
All major sections may have some sub components used for organization
The below list is major components only

ctrl + f #. ClassName

0. General GUI
1. GuiParams - parameters that power the note selection algorithm
2. GuiSpectrum - customized frequency spectrum using logic in DCT.cpp
3. GuiWindow - View into the window used to generate GuiSpectrum and amplitude data
3. GuiLog - print out of all useful info generated within the audioProcessBlock
4. GuiSettings - user settings/saving/loading etc.
5. GuiTabs - holds everything together
*/


//0. General GUI ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


SliderPanel::SliderPanel(const char* panelName, int sliderCount, bool showPanelDesc) :
    sliders{ sliderCount }, labels{ sliderCount } 
{
    this->showPanelDesc = showPanelDesc;
    initText(desc, panelName, 36);
    if(showPanelDesc)
    {
        addAndMakeVisible(desc);
    }

    APPLY_FUNC_TO_ELEM(addAndMakeVisible, sliders);
    APPLY_FUNC_TO_ELEM(addAndMakeVisible, labels);
    CALL_EACH_ELEM_FUNC(sliders, setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, true, 0, 0));
    CALL_EACH_ELEM_FUNC(labels, setJustificationType(juce::Justification(36)) );
    

}

void SliderPanel::setLabelNames(std::vector<const char*> names) 
{
    CALL_EACH_ELEM_FUNC(labels, setText(names[i], juce::NotificationType::dontSendNotification))
}

void SliderPanel::setSliderDescriptions(std::vector<const char*> names) 
{
    CALL_EACH_ELEM_FUNC(sliders,setName(names[i]))
}

void SliderPanel::setRanges(std::vector<juce::Range<double>> ranges, std::vector<double> intervals)
{
    CALL_EACH_ELEM_FUNC(sliders, setRange(ranges[i], intervals[i]))
}

void SliderPanel::setSliderStyles(std::vector < juce::Slider::SliderStyle> styles)
{
    CALL_EACH_ELEM_FUNC(sliders, setSliderStyle(styles[i]))
}

void SliderPanel::setSliderTextBoxes(std::vector<juce::Slider::TextEntryBoxPosition> positions)
{
    CALL_EACH_ELEM_FUNC(sliders, setTextBoxStyle(positions[i], true, 0, 0))
}

void SliderPanel::resized()
{
    auto area = getLocalBounds();
    resizeH(area, (int)getHeight() * 0.05f);

    desc.setBounds(area.removeFromLeft(getWidth() * .2));

    auto sliderArea = area.removeFromTop(getHeight() * 0.75f);
    for (int i = 0; i < sliders.size(); i++)
    {
        int w = getWidth() * 0.1f;
        int boxh = sliderArea.getHeight() * .15f;
        sliders[i].setTextBoxStyle(juce::Slider::TextBoxAbove, true, w, boxh);
        sliders[i].setBounds(sliderArea.removeFromLeft(w));
        labels[i].setBounds(area.removeFromLeft(w));
    }

}


//1. GuiParams ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//temporary macros to test things out

GuiParams::GuiParams()
{
    setSize(REFX, REFY);

    addAndMakeVisible(status);
    initText(status, defaultStatus, 36);
    
    addAndMakeVisible(noteID);
    addAndMakeVisible(amplitude);
    addAndMakeVisible(midi);
    
    //NoteID Panel~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    noteID.setLabelNames(std::vector<const char*>{"Low Oct", "Oct Str", "Trigger", "Retrig On", "Retrig Off"});

    noteID.setRanges(
        std::vector<juce::Range<double>>{
            juce::Range<double>(0,3), 
            juce::Range<double>(0,5),
            juce::Range<double>(0,100),
            juce::Range<double>(80, 120),
            juce::Range<double>(80,120) },
        std::vector<double>{1, 0.5, 0.5, 1, 1});

    noteID.setSliderStyles(std::vector < juce::Slider::SliderStyle>{
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical, 
        juce::Slider::SliderStyle::LinearVertical});


    //Amp Panel~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    amplitude.setLabelNames(std::vector<const char*>{"Noise", "Release"});

    amplitude.setRanges(
        std::vector<juce::Range<double>>{
        juce::Range<double>(-90, 0),
        juce::Range<double>(-90, 0)},
        std::vector<double>{1, 1});

    amplitude.setSliderStyles(std::vector < juce::Slider::SliderStyle>{
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical});

    //Midi Panel~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    midi.setLabelNames(std::vector<const char*>{
        "Oct", "Semi", 
        "dB Map Max", "dB Map Min",
        "Vel Map Max", "Vel Map Min"});

    midi.setRanges(
        std::vector<juce::Range<double>>{
        juce::Range<double>(-8, 8),
        juce::Range<double>(-11, 11),
        juce::Range<double>(-90, 0),
        juce::Range<double>(-90, 0),
        juce::Range<double>(0, 127),
        juce::Range<double>(0, 127) },
        std::vector<double>{1, 1, 1, 1, 1, 1});

    midi.setSliderStyles(std::vector < juce::Slider::SliderStyle>{
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical,
        juce::Slider::SliderStyle::LinearVertical});
}

void GuiParams::resized()
{
    auto area = getLocalBounds();
    float sectionH = getHeight() * 1.0f/panels.size();

    for (int i = 0; i < panels.size(); i++)
    {
        panels[i]->setBounds(area.removeFromTop(sectionH));
    }
    
}



//2. GuiSpectrum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiSpectrum::GuiSpectrum() {}
void GuiSpectrum::resized() {}

//3. GuiWindow ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiWindow::GuiWindow() {}
void GuiWindow::resized() {}

//4. GuiLog ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiLog::GuiLog()
{
    std::vector<const char*> text{
        "f0 Ind", "Oct Str", "Oct", "Pitch",
        "Note", "Oct Str", "Oct", "Pitch",
        "Amp", "dB", "Trig", "Retrig",
        "Midi On", "On Vel", "Midi Off" , "Off Vel"
    };

    for (int i = 0; i < nameLabels.size(); i++) 
    {
        initText(nameLabels[i], text[i], 36);
        initText(valueLabels[i], "0", 36);
    }

    freqTitle.setText("Frequency", juce::NotificationType::dontSendNotification);
    ampTitle.setText("Waveform", juce::NotificationType::dontSendNotification);
    midiTitle.setText("Midi", juce::NotificationType::dontSendNotification);

    APPLY_FUNC_TO_ELEM(addAndMakeVisible, nameLabels);
    APPLY_FUNC_TO_ELEM(addAndMakeVisible, valueLabels);

    addAndMakeVisible(freqTitle);
    addAndMakeVisible(ampTitle);
    addAndMakeVisible(midiTitle);

}

void GuiLog::setValueLabels(int f0ind, float f0octStr, int note, float noteOctStr,
    float amp, float dB, float trigger, float retrigger,
    int midiOn, int midiOff, int velOn, int velOff) 
{
    //place holder
}

#define RESIZE_LOG_LABELS(panel, start, stop, size) {\
    w = panel.getWidth() / size;\
    for (int i = start; i < stop; i++)\
    {\
        auto section = panel.removeFromLeft(w);\
        nameLabels[i].setBounds(section.removeFromTop(section.getHeight() * 0.5f));\
        valueLabels[i].setBounds(section);\
    }\
}
void GuiLog::resized() 
{
    auto area = getLocalBounds();
    auto panelH = area.getHeight() *0.6f / 4;
    auto titleH = area.getHeight() * 0.4f / 4;

    auto title1 = area.removeFromTop(titleH);
    auto panel1a = area.removeFromTop(panelH);
    auto panel1b = area.removeFromTop(panelH);

    auto title2 = area.removeFromTop(titleH);
    auto panel2 = area.removeFromTop(panelH);

    auto title3 = area.removeFromTop(titleH);
    auto panel3 = area.removeFromTop(panelH);

    
    freqTitle.setBounds(title1);
    ampTitle.setBounds(title2);
    midiTitle.setBounds(title3);

    int w = 0;

    RESIZE_LOG_LABELS(panel1a, 0, 4, 4);
    RESIZE_LOG_LABELS(panel1b, 4, 8, 4);
    RESIZE_LOG_LABELS(panel2, 8, 12, 4);
    RESIZE_LOG_LABELS(panel3, 12, 16, 4);

}

//5. GuiSettings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiSettings::GuiSettings() {}
void GuiSettings::resized() {}

//6. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
