/*
  ==============================================================================

    GUITabs.cpp
    Created: 4 Dec 2020 11:44:58am
    Author:  cjack

  ==============================================================================
*/

#include "GUITabs.h"

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