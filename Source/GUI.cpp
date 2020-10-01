
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
        case 3:
            guiState = (GUIState)3;
            break;
        case 4:
            guiState = (GUIState)4;
            break;
    }
}

PaperLookAndFeel::PaperLookAndFeel() 
{
    setColour(0x1000280, PAPER);
    setColour(0x1000281, BOLD_BLACK_INK);
    setColour(0x1000282, FADE_BLACK_INK);

    setColour(0x1001200, PAPER);
    setColour(0x1001300, BOLD_BLACK_INK);
    setColour(0x1001310, FADE_BLACK_INK);

    setColour(0x1005800, PAPER_DARK);
    setColour(0x1005801, FADE_BLACK_INK);

    setColour(0x1005812, FADE_BLACK_INK);
    setColour(0x1005813, FADE_BLACK_INK);
    setColour(0x1005814, BOLD_BLACK_INK);
    setColour(0x1005815, BOLD_BLACK_INK);
}