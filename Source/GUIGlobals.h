/*
  ==============================================================================

    GUIGlobals.h
    Created: 1 Oct 2020 2:53:48pm
    Author:  cjack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


//default x and y values for the project which other screen sizes are based around
//16:9 res
const int REFX = 800;
const int REFY = 450;

//paper color theme
const juce::Colour PAPER{ (juce::uint8)205, (juce::uint8)198, (juce::uint8)177 };
const juce::Colour PAPER_DARK{ (juce::uint8)183, (juce::uint8)173, (juce::uint8)143 };
const juce::Colour PAPER_LIGHT{ (juce::uint8)213, (juce::uint8)207, (juce::uint8)190 };

const juce::Colour BOLD_RED_INK{ (juce::uint8)103, (juce::uint8)15, (juce::uint8)9 };
const juce::Colour FADE_RED_INK{ (juce::uint8)98, (juce::uint8)53, (juce::uint8)50 };

const juce::Colour BOLD_GREEN_INK{ (juce::uint8)21, (juce::uint8)73, (juce::uint8)23 };
const juce::Colour FADE_GREEN_INK{ (juce::uint8)51, (juce::uint8)77, (juce::uint8)52 };

const juce::Colour BOLD_BLUE_INK{ (juce::uint8)21, (juce::uint8)30, (juce::uint8)97 };
const juce::Colour FADE_BLUE_INK{ (juce::uint8)53, (juce::uint8)58, (juce::uint8)95 };

const juce::Colour BOLD_BLACK_INK{ (juce::uint8)0, (juce::uint8)0, (juce::uint8)0 };
const juce::Colour FADE_BLACK_INK{ (juce::uint8)36, (juce::uint8)36, (juce::uint8)36 };

const juce::Colour MARKER0{ (juce::uint8)0, (juce::uint8)0, (juce::uint8)0 };
const juce::Colour MARKER1{ (juce::uint8)64, (juce::uint8)64, (juce::uint8)64 };
const juce::Colour MARKER2{ (juce::uint8)128, (juce::uint8)128, (juce::uint8)128 };
const juce::Colour MARKER3{ (juce::uint8)192, (juce::uint8)192, (juce::uint8)192 };


//implement this as a pluginProcessor field later
//pass the plugin processor to the guitab;
enum class GUIState { main, spectrum, signal };

#define CALL_EACH_ELEM_FUNC(vec, func) {\
for(int i = 0; i < vec.size(); i++)\
    {\
        vec[i].func;\
    }\
}

#define APPLY_FUNC_TO_ELEM(func, vec) {\
for(int i = 0; i < vec.size(); i++)\
    {\
        func(vec[i]);\
    }\
}

#define REMOVE_FROM_ALL_SIDES(area, val){\
area.removeFromLeft(val);\
area.removeFromRight(val);\
area.removeFromTop(val);\
area.removeFromBottom(val);\
}


