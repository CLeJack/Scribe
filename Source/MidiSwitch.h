/*
  ==============================================================================

    MidiSwitch.h
    Created: 24 Oct 2020 5:16:23pm
    Author:  cjack

  ==============================================================================
*/


#pragma once
#include "Head.h"
#include "Stats.h"

enum class MidiState {off, on, retrigger};

struct MidiMode{};
struct ConstrainMidi : public MidiMode {};

struct MidiParams
{
    float weightVal = 0;
    float weightThresh = 0;

    float midiNum = 0;
    float delaydB = 0;
    float noiseThresh = 0;
    float releaseThresh = 0;

    float retrigVal = 0;
    float retrigStart = 0;
    float retrigSameStart = 0;
    float retrigStop = 0;
    
    int velocityVal;
    
    float smoothFactor = 0;
    bool isConsistent = false;
};

struct SwitchMessage
{
    int on = 0;
    int onVel = 0;
    int off = 0;
    int offVel = 0;
    bool send = false;
};

struct Notes
{
    void push(float val);
    void clear();
    bool areEqual();
    bool equal(float midiNum);
    bool areZero();
    float current = 0;
    float prev = 0;
};

struct MidiSwitch
{
    /*
    ivec structure 
    */
    SwitchMessage update(const MidiParams& params);
    
    //received values from the above update functions;
    /*
    * To remain consistent, the following rules should apply to midi state
    * init state and on state have arbitrary complexity and may have multiple user selectable algorithms internally
    * -off state and retrigger state watches amplitude and trigger metrics only
    * 
    */
    SwitchMessage on  (const MidiParams& params);
    SwitchMessage off (const MidiParams& params);
    SwitchMessage retrigger (const MidiParams& params);

    void onSequence(const MidiParams& p, SwitchMessage& m);
    void offSequence(const MidiParams& p, SwitchMessage& m);

    

    static int getMidiNumber(float freq, float refFreq)
    {
        return int(0.5f + 69 + 12 * std::log2(freq / refFreq));
    }

    int getVelocity(const MidiParams& params);

    float smoothNote(const MidiParams& params);

    MidiState state = MidiState::off;
    int initCounter = 0;
    
    Notes notes = Notes();
};




inline int midiRound(float val)
{
    return int(0.5 + val);
}

inline int midiShift(int f0ind, const fvec& freqs, float refFreq, int octShift, int semitoneShift)
{
    if(f0ind <= 0)
    {
        return 0;
    }
    
    int midi = MidiSwitch::getMidiNumber(freqs[f0ind], refFreq);
    int oct = octShift + midi/12;
    int pitch = (midi%12) + semitoneShift;

    int out = 12*oct + pitch;

    if(out < 0 || out > 127)
    {
        return 0;
    }
    
    return out;
}

inline int midiVelocity(float maxVel, float minVel, float dBShort, float dBLong, float velRatio) 
{

    if (maxVel == minVel) 
    {
        return maxVel;
    }

    if(dBShort < dBLong)
    {
        return (maxVel - minVel)/2;
    }

    //(dBShort - dBLong) / dt = velRatio * (currentVel - minVel)/dt


    int vel = (dBShort - dBLong) / velRatio + minVel;
    vel = vel > maxVel ? maxVel : vel;
    vel = vel < minVel ? minVel : vel;

    return vel;
}
