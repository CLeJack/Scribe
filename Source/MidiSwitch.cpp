#include "MidiSwitch.h"

MidiSwitch::MidiSwitch() {}

MidiSwitch::MidiSwitch(int index, float freq, float refFreq) 
{
    this->index = index;
    this->midiNum = getMidiNumber(freq, refFreq);
}

void MidiSwitch::turnOn(const MidiParams& params, const fvec& maxWeights, const fvec& certainty)
{
    currentdB = weightTodB(maxWeights[index], params.refdB);
    this->certainty = certainty[index];
    isOn = true;

    onNote = midiShift(params);
    onVel = calculateVelocity(params);
}

void MidiSwitch::update(const MidiParams& params, const fvec& maxWeights, const fvec& certainty)
{
    //I'm not liking this offset--look into alternative methods later.
    currentdB = weightTodB(maxWeights[index], params.refdB);
    this->certainty = certainty[index];
}

bool MidiSwitch::needsRelease(const MidiParams& params)
{

    bool output = false;
    if(isOn)
    {
        if( currentdB < params.releaseThresh)
        {
            output = true;
            isOn = false;
        }

    }
    

    return output;
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

    //switch this back to previous method with rolling values
    float pct = std::abs(
        (params.refdB - params.releaseThresh) 
        / (params.maxdB - params.releaseThresh)
        );

    int vel = params.minVel + pct * (params.maxVel - params.minVel);
    vel = vel > params.maxVel ? params.maxVel : vel;
    vel = vel < params.minVel ? params.minVel : vel;

    return vel;
}

MidiSwitch& MidiSwitch::operator=(MidiSwitch other)
{
    index = other.index;
    midiNum = other.midiNum;
    return *this;
}