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
    
    float diff = std::abs(params.velDbMax - params.velDbMin);
    float pct = std::abs(params.ampdB - params.velDbMin)/diff;
    
    int vel = pct*(params.velMax - params.velMin) + params.velMin;
    vel = vel > params.velMax ? params.velMax : vel;
    vel = vel < params.velMin ? params.velMin : vel;

    //std::cout<<params.amp<< ", "<<diff<< ", "<<pct<< ", "<<vel<<"\n";

    return vel;
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



float MidiSwitch::SMA(float prev_avg, float current_val, int size)
{
    /*
    it seems this method has two vunerabilities

    1. it holds on to data introduced into the history much longer than if an actual window was used
       e.g. if a million was introduced with a prev_avg of 1, many updates would be needed to flush a million
       if a window of size N with history was used, only N+1 updates would be needed.

    2. rounding error is introduced with each division.

    neither should be an issue here
    */
    float avg = prev_avg - prev_avg / size;
    avg += current_val / size;
    return avg;
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