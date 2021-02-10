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
        case MidiState::bend :
            output = bend(params);
            break;
    }
    return output;
}

void MidiSwitch::onSequence(const MidiParams& p, SwitchMessage& m)
{
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
    else if(notes.current != params.midiNum && params.isConsistent && params.bendOn
        && params.frequencyDelta < params.bendThreshold)
    {
        //This state needs to come before retrigger
        //string bending can cause fluctions of the retrigger value 
        //so these two states overlap
        //notes.push(params.midiNum);
        //onSequence(params, output);
        state = params.retrigVal < params.retrigSameStart? MidiState::retrigger : MidiState::bend;

    }
    else if ((params.retrigVal < params.retrigStart && notes.current != params.midiNum && !params.bendOn) 
        || (params.retrigVal < params.retrigSameStart && notes.current == params.midiNum))
    {
        //retrigger activates much more quickly than bend detection
        //it can't be allowed to activate with default parameters when the bend state exists
        state = MidiState::retrigger;

    }
    else if (notes.current != params.midiNum && params.isConsistent)
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
    if (!notes.areZero())
    {   
        //clear midiNum buffer if it hasn't been cleared out;

        offSequence(params, output);
        //order matters here.
        //don't want to push a new midiNum before setting the noteOff midiNum
        notes.push(0);
        
    }
    else 
    {
        state = MidiState::off;
    }
    return output;
}

SwitchMessage MidiSwitch::bend(const MidiParams & params)
{
    SwitchMessage output = SwitchMessage();

    if(params.delaydB < params.releaseThresh)
    {
        state = MidiState::off;
    }
    //else if ((params.retrigVal < params.retrigSameStart && notes.current != params.midiNum)
    //    || (params.retrigVal < params.retrigSameStart && notes.current == params.midiNum))
    else if(params.frequencyDelta > params.bendThreshold)
    {
        state = MidiState::retrigger;

    }

    output.send = false;

    return output;
}

float MidiSwitch::smoothNote(const MidiParams& params)
{
    //acting like a rolling average where the window size = smoothFactor
    if(params.midiNum == 0)
    {
        return notes.current;
    }
    
    return SMABlocks(notes.current, params.midiNum, params.smoothFactor);
}