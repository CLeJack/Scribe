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

int MidiSwitch::getVelocity(const MidiParams& params)
{
    if(params.velMax == params.velMin)
    {
        return params.velMax;
    }
    
    /*
    float diff = std::abs(params.velDbMax - params.velDbMin);
    float pct = std::abs(params.ampdB - params.velDbMin)/diff;
    
    int vel = pct*(params.velMax - params.velMin) + params.velMin;
    vel = vel > params.velMax ? params.velMax : vel;
    vel = vel < params.velMin ? params.velMin : vel;
    */

    int vel = params.velPTheta * params.velocityAngle + params.velMin;
    return std::min(vel, params.velMax);
}

void MidiSwitch::onSequence(const MidiParams& p, SwitchMessage& m)
{
    // if a midiNum switches too rapidly without a retrigger--don't count it.
    // legato and slides happen over a time period that should be detectable by retrigger
    // or portamento logic
    m.on = midiRound(notes.current);

    m.onVel = getVelocity(p);
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
    if(params.ampdB < params.releasedB )
    {
        state = MidiState::off;
    }
    else if(params.retrig < params.retrigStart)
    {
        state = MidiState::retrigger;
    }
    else if(!notes.equal(smooth))
    {
        //portamento or retrigger logic here.
        //going to retrigger for now
        notes.push(smooth);
        onSequence(params, output);

    }
    else
    {
        notes.push(smooth);
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
    else if(params.ampdB > params.noisedB && params.midiNum != 0)
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
    if(!notes.areZero() && params.retrig < params.retrigStop)
    {   
        //clear midiNum buffer if it hasn't been cleared out;

        offSequence(params, output);
        //order matters here.
        //don't want to push a new midiNum before setting off midiNum to prev
        notes.push(0);
        
    }
    else if(params.retrig >= params.retrigStop  || params.ampdB < params.releasedB)
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
    float ref = notes.current * (1.0f - 1.0f/params.smoothFactor);
    ref += params.midiNum * 1.0f/params.smoothFactor;
    ref = std::abs(notes.current - ref) < params.constrainStep ? ref : notes.current;
    return ref;
}