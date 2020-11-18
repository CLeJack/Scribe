/*
  ==============================================================================

    MidiSwitch.cpp
    Created: 24 Oct 2020 5:16:23pm
    Author:  cjack

  ==============================================================================
*/


#include "MidiSwitch.h"


void Notes::push(float val)
{
    prev = current;
    current = val;
}

void Notes::clear()
{
    prev = 0;
    current = 0;
}

bool Notes::areEqual()
{
    return midiRound(prev) == midiRound(current);
}

bool Notes::equal(float midiNum)
{
    return midiRound(midiNum) == midiRound(current);
}

bool Notes::areZero()
{
    return midiRound(prev) + midiRound(current) == 0;
}

SwitchMessage MidiSwitch::update(const MidiParams& params)
{
    SwitchMessage output = SwitchMessage();

    switch(state)
    {
        case MidiState::on :
            output = on(params);
            break;
        case MidiState::retrigger :
            output = retrigger(params);
            break;
        case MidiState::off :
            output = off(params);
            break;
    }
    return output;
}

void MidiSwitch::onSequence(const MidiParams& p, SwitchMessage& m)
{
    // if a midiNum switches too rapidly without a retrigger--don't count it.
    // legato and slides happen over a time period that should be detectable by retrigger
    // or portamento logic
    m.on = midiRound(notes.current);

    m.onVel = p.velocityVal;
    m.off = midiRound(notes.prev);
    m.offVel = 0;
    m.send = true;
}
void MidiSwitch::offSequence(const MidiParams& p, SwitchMessage& m)
{
    m.on = 0;
    m.onVel = 0;
    m.off = midiRound(notes.prev);
    m.offVel = 0;
    m.send = true;
}

SwitchMessage MidiSwitch::on (const MidiParams& params)
{
    SwitchMessage output = SwitchMessage();
    float smooth;
    
    smooth = smoothNote(params); //should be called before on, retrigger
    if(params.delaydB < params.releaseThresh )
    {
        state = MidiState::off;
    }
    else if( (params.retrigVal < params.retrigStart && notes.current != params.midiNum) 
        || (params.retrigVal < params.retrigSameStart && notes.current == params.midiNum) )
    {
        state = MidiState::retrigger;
        
    }
    else if(notes.current != params.midiNum && params.isConsistent)
    {
        notes.push(params.midiNum);
        onSequence(params, output);

    }

    return output;
}


SwitchMessage MidiSwitch::off (const MidiParams& params)
{
    SwitchMessage output = SwitchMessage();
    //if(!notes.areZero() && !validAmp(params))
    if(!notes.areZero())
    {   
        //clear midiNum buffer if it hasn't been cleared out;

        offSequence(params, output);
        //order matters here.
        //don't want to push a new midiNum before setting off midiNum to prev
        notes.push(0);
        
    }
    else if(params.delaydB > params.noiseThresh && params.midiNum != 0
            && params.isConsistent)
    {
        notes.push(params.midiNum);
        onSequence(params, output);
        state = MidiState::on;

    }

    return output;
}


SwitchMessage MidiSwitch::retrigger (const MidiParams& params)
{
    SwitchMessage output = SwitchMessage();
    //if(!notes.areZero() && params.retrigVal < params.retrigStop)
    if (!notes.areZero())
    {   
        //clear midiNum buffer if it hasn't been cleared out;

        offSequence(params, output);
        //order matters here.
        //don't want to push a new midiNum before setting off midiNum to prev
        notes.push(0);
        
    }
    //else if(params.retrigVal >= params.retrigStop  || params.delaydB < params.releaseThresh)
    else 
    {
        state = MidiState::off;
    }
    return output;
}

float MidiSwitch::smoothNote(const MidiParams& params)
{
    //acting like a rolling average where the window size = smoothFactor
    if(params.midiNum == 0)
    {
        return notes.current;
    }
    
    return SMABlocks(notes.current, params.midiNum, params.smoothFactor);;
}