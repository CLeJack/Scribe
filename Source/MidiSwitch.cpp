#include "MidiSwitch.h"

MidiSwitch::MidiSwitch() {}

MidiSwitch::MidiSwitch(int index, float freq, float refFreq) 
{
}

void MidiSwitch::turnOn(const MidiParams& params, const fvec& maxWeights, const fvec& certainty)
{
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



MidiSwitch& MidiSwitch::operator=(MidiSwitch other)
{
    index = other.index;
    midiNum = other.midiNum;
    return *this;
}