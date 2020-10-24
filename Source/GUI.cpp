
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



#define DB_Y(Y, val, dBrange) {\
    Y = val / dBrange;\
    Y = Y > 0 ? 0 : Y;\
    Y = Y < -1 ? -1 : Y;\
    Y = floor - (H * (Y + 1));\
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
    }
}

PaperLookAndFeel::PaperLookAndFeel() 
{

    //Label
    setColour(0x1000280, PAPER);
    setColour(0x1000281, BOLD_BLACK_INK);
    setColour(0x1000282, juce::Colours::black.withAlpha(0.0f));

    //resizable window
    setColour(0x1005700, PAPER);


    //slider
    setColour(0x1001200, PAPER_DARK);
    setColour(0x1001300, FADE_BLACK_INK);
    setColour(0x1001310, PAPER_DARK);
    setColour(0x1001311, FADE_BLACK_INK);
    setColour(0x1001312, PAPER_DARK);
    setColour(0x1001312, PAPER_DARK);

    
    //TabbedComponent
    setColour(0x1005800, PAPER_DARK);
    setColour(0x1005801, FADE_BLACK_INK);

    //Tabbed ButtonBar
    setColour(0x1005812, FADE_BLACK_INK);
    setColour(0x1005813, FADE_BLACK_INK);
    setColour(0x1005814, BOLD_BLACK_INK);
    setColour(0x1005815, BOLD_BLACK_INK);
    

    //TextButton
    setColour(0x1000100, BOLD_BLACK_INK);
    setColour(0x1000101, FADE_BLACK_INK);
    
}

/*


//--- Label

backgroundColourId                  = 0x1000280, PAPER

textColourId                        = 0x1000281, BOLD_BLACK_INK

outlineColourId                     = 0x1000282, FADE_BLACK_INK


//Resizable window

backgroundColourId                  = 0x1005700, PAPER

//--- Slider

backgroundColourId                  = 0x1001200, PAPER_DARK

thumbColourId                       = 0x1001300, BOLD_BLACK_INK

trackColourid                       = 0x1001310, PAPER_DARK

rotaryFill                          = 0x1001311, FADE_BLACK_INK //fills on the left side 

rotarySliderOutlineColourId         = 0x1001312, PAPER_DARK

//--- TabbedComponent

backgroundColourId                  = 0x1005800, PAPER_DARK

outlineColourId                     = 0x1005801, FADE_BLACK_INK



//--- TabbedButtonBar

tabOutlineColourId                  = 0x1005812, FADE_BLACK_INK

tabTextColourId                     = 0x1005813, FADE_BLACK_INK

frontOutlineColourId                = 0x1005814, BOLD_BLACK_INK

frontTextColourId                   = 0x1005815, BOLD_BLACK_INK


//--- TextButton
buttonColourId                      = 0x1000100, BOLD_BLACK_INK

buttonOnColourId                    = 0x1000101, FADE_BLACK_INK

*/