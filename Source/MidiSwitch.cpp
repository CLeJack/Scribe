#include "MidiSwitch.h"


MidiSwitch::MidiSwitch(int index) : index(index) {}

void MidiSwitch::turnOn(const MidiParams& params, const fvec& maxWeights)
{
    shortdB = maxWeights[index+lowestNote] * params.refdB;
    longdB = shortdB;

    isOn = true;

    onNote = midiShift(params);
    onVel = calculateVelocity(params);
}

void MidiSwitch::update(const MidiParams& params, const fvec& maxWeights)
{
    float dB = maxWeights[index+lowestNote] * params.refdB;
    shortdB = SMA(shortdB, dB, params.smoothingBlocks);
    longdB = SMA(shortdB, dB, params.smoothingBlocks * 2);
}

bool MidiSwitch::needsRelease(const MidiParams& params)
{
    retrigPct = shortdB/longdB;
    bool output = false;
    if( isOn && (retrigPct < params.retrigStart || params.refdB < params.releaseThresh) )
    {
        output = true;
        isOn = false;
    }

    return output;
}

void MidiSwitch::updateLowNote(int lowNote, const fvec& freqs, float refFreq)
{
    if(lowestNote != lowNote)
    {
        lowestNote = lowNote;
        midiNum = getMidiNumber(freqs[index + lowestNote], refFreq);
    }
}

int MidiSwitch::midiShift(const MidiParams& params)
{
    int midi = midiNum;
    int oct = params.octShift + midi/12;
    int pitch = (midi%12) + params.semiShift;

    int out = 12*oct + pitch;

    if(out < 0 || out > 127)
    {
        return 0;
    }
    
    return out;
}





int MidiSwitch::calculateVelocity(const MidiParams& params) 
{

    float pct = (params.refdB - params.releaseThresh) 
                / (params.maxdB - params.releaseThresh);

    int vel = params.minVel + pct * (params.maxVel - params.minVel);
    vel = vel > params.maxVel ? params.maxVel : vel;
    vel = vel < params.minVel ? params.minVel : vel;

    return vel;
}