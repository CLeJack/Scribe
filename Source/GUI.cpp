
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
GuiSpectrum::GuiSpectrum() : data(48, 1)
{
    addAndMakeVisible(oct);
    addAndMakeVisible(pitch);
}

inline void displayBars(juce::Rectangle<float>& display, juce::Rectangle<float>& bar, std::vector<float>& data,
    juce::Colour colour, juce::Colour hilight,
    juce::Graphics& g) 
{
    g.setColour(colour); 
    
    float W = (display.getRight() - display.getX()) / data.size(); 
    float H = 0; 
    float X = display.getX(); 
    float Y = display.getBottom(); 
    float max = maxValue(data); 
        
    for (int i = 0; i < data.size(); i++)
    {
        //values range from .001 to 1 giving a range of -3 to 0 for log10
        H = data[i] / max; //attempt [0,1] constraint
        H = H > 1 ? 1 : H;
        H = H < 0 ? 0 : H; //enforce constraint
        H *= display.getHeight();

        bar.setBounds(X, Y - H, W, H);
        if (data[i] == max)
        {
            g.setColour(hilight);
        }
        g.fillRect(bar);

        g.setColour(colour);
        X += W;
    }
}

void GuiSpectrum::paint(juce::Graphics& g)
{
    //const juce::MessageManagerLock mmLock;
    //bg ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    g.setColour(PAPER);
    g.fillAll(PAPER);
    float W = getWidth();
    float H = getHeight() ;
    float X = 0;
    float Y = 0;
    float outerPad = H * 0.05f;
    
    //display section ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    juce::Rectangle<float> display(X, Y, W, H);
    display.removeFromTop(H * 0.3f);
    display.removeFromTop(outerPad);
    display.removeFromBottom(outerPad);
    display.removeFromLeft(outerPad);
    display.removeFromRight(outerPad);

    g.setColour(BOLD_BLACK_INK);
    g.drawRect(display, 2.0f);

    juce::Rectangle<float> bar;
 
    displayBars(display, bar, data, FADE_BLACK_INK, BOLD_BLACK_INK, g);

}

void GuiSpectrum::resized()
{
    auto area = getLocalBounds() * 0.25f;
    int pad = area.getHeight() * 0.05f;

    area.removeFromTop(pad);
    area.removeFromBottom(pad);
    area.removeFromLeft(pad);
    area.removeFromRight(pad);

    int w = area.getWidth() * 0.2f;

    oct.setBounds(area.removeFromLeft(w));
    area.removeFromLeft(pad);
    pitch.setBounds(area.removeFromLeft(w));

}

//3. GuiWindow ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GuiWindow::GuiWindow() : dBBuff(60, 0), signalVec(201,0)
{
}

void GuiWindow::resized() {}


#define DB_Y(Y, val, dBrange) {\
    Y = val / dBrange;\
    Y = Y > 0 ? 0 : Y;\
    Y = Y < -1 ? -1 : Y;\
    Y = floor - (H * (Y + 1));\
}
void GuiWindow::paint(juce::Graphics& g) 
{
    const juce::MessageManagerLock mmLock;
    g.fillAll(PAPER);
    juce::Path dBPath;
    juce::Path signalPath;
    juce::PathStrokeType stroke(2.0f);

    dBPath.preallocateSpace(dBBuff.size * 3);
    signalPath.preallocateSpace(signalVec.size() * 3);
    

    float W = getWidth();
    float H = getHeight();
    float X = 0;
    float Y = 0;
    float outerPad = H * 0.1f;
    juce::Rectangle<float> panel1(outerPad, outerPad, W - 2 * outerPad, H - 2 * outerPad);

    W = panel1.getWidth();
    H = panel1.getHeight();
    X = panel1.getX();
    Y = panel1.getY();
    
    auto panel0 = panel1.removeFromTop(H * 0.4f);
    panel1.removeFromTop(H * 0.2f);

    g.setColour(BOLD_BLACK_INK);

    g.drawRect(panel0);
    g.drawRect(panel1);

    //add amplitude data to path

    W = panel0.getWidth() / (float)dBBuff.size;
    H = panel0.getHeight();
    X = panel0.getX();
    float floor = panel0.getBottom();
    Y = 0;
    DB_Y(Y, dBBuff.currentValue(), 60.0f)


    dBPath.startNewSubPath(X, Y);
    for (int i = dBBuff.head + 1; i < dBBuff.size; i++) 
    {
        X += W;
        DB_Y(Y, dBBuff.vec[i], 60.0f)
        dBPath.lineTo(X, Y);
    }

    for (int i = 0; i < dBBuff.head; i++) 
    {
        X += W;
        DB_Y(Y, dBBuff.vec[i], 60.0f)
        dBPath.lineTo(X, Y);
    }

    g.strokePath(dBPath, stroke);

    //add signal data to path

    W = panel1.getWidth() / (float)signalVec.size();
    H = panel1.getHeight();
    X = panel1.getX();
    floor = panel1.getBottom();
    float max = absMaxValue(signalVec);
    Y = (signalVec[0] + max) / (2*max);
    Y = Y > 1 ? 1 : Y;
    Y = floor - H * Y;

    signalPath.startNewSubPath(X, Y);
    
    for (int i = 1; i < signalVec.size(); i++) 
    {
        X += W;
        Y = (signalVec[i] + max) / (2 * max);
        Y = Y > 1 ? 1 : Y;
        Y = floor - H * Y;
        signalPath.lineTo(X, Y);
    }

    g.strokePath(signalPath, stroke);



}

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

void GuiLog::setValueLabels(int f0ind, float f0ratio, int f0oct, int f0pitch,
    int noteInd, float noteRatio, int noteOct, int notePitch,
    float amp, float ampdB, float trigger, float retrigger,
    int midiOn, int velOn, int midiOff, int velOff)
{
    valueLabels[0].setText(std::to_string(f0ind), juce::NotificationType::dontSendNotification);
    valueLabels[1].setText(std::to_string(f0ratio), juce::NotificationType::dontSendNotification);
    valueLabels[2].setText(std::to_string(f0oct), juce::NotificationType::dontSendNotification);
    valueLabels[3].setText(std::to_string(f0pitch), juce::NotificationType::dontSendNotification);
    valueLabels[4].setText(std::to_string(noteInd), juce::NotificationType::dontSendNotification);
    valueLabels[5].setText(std::to_string(noteRatio), juce::NotificationType::dontSendNotification);
    valueLabels[6].setText(std::to_string(noteOct), juce::NotificationType::dontSendNotification);
    valueLabels[7].setText(std::to_string(notePitch), juce::NotificationType::dontSendNotification);
    valueLabels[8].setText(std::to_string(amp), juce::NotificationType::dontSendNotification);
    valueLabels[9].setText(std::to_string(ampdB), juce::NotificationType::dontSendNotification);
    valueLabels[10].setText(std::to_string(trigger), juce::NotificationType::dontSendNotification);
    valueLabels[11].setText(std::to_string(retrigger), juce::NotificationType::dontSendNotification);
    valueLabels[12].setText(std::to_string(midiOn), juce::NotificationType::dontSendNotification);
    valueLabels[13].setText(std::to_string(velOn), juce::NotificationType::dontSendNotification);
    valueLabels[14].setText(std::to_string(midiOff), juce::NotificationType::dontSendNotification);
    valueLabels[15].setText(std::to_string(velOff), juce::NotificationType::dontSendNotification);
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
void GuiSettings::resized()
{
}

//6. GuiTabs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void GuiTabs::currentTabChanged(int ind, const juce::String& name) 
{
    switch (ind) 
    {
        case 0 :
            guiState = (GUIState)0;
            break;
        case 1 :
            guiState = (GUIState)1;
            break;
        case 2 :
            guiState = (GUIState)2;
            break;
        case 3:
            guiState = (GUIState)3;
            break;
        case 4:
            guiState = (GUIState)4;
            break;
    }
}