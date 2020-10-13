#pragma once
#include "Head.h"
#include "Stats.h"

enum class MidiState {off, on, retrigger};

struct MidiMode{};
struct ConstrainMidi : public MidiMode {};

struct MidiParams
{
    float certaintyThresh = 0;
    float releaseThresh = 0;

    float retrigStart = 0;
    float retrigStop = 0;
    
    float smoothingBlocks = 0;

    int octShift = 0;
    int semiShift = 0;

    float refdB = 0;
    
    float maxdB = 0;
    int maxVel = 0;
    int minVel = 0;

    int lowNote = 0;
    int highNote = 0;
};

struct MidiSwitch
{
    MidiSwitch();
    MidiSwitch(int index, float freq, float refFreq);

    void turnOn  (const MidiParams& params, const fvec& maxWeights, const fvec& certainty);

    void update(const MidiParams& params, const fvec& maxWeights, const fvec& certainty);
    bool needsRelease (const MidiParams& params);

    int calculateVelocity(const MidiParams& params);
    int midiShift(const MidiParams& params);
    

    int getMidiNumber(float freq, float refFreq)
    {
        return int(0.5f + 69 + 12 * std::log2(freq / refFreq));
    }

    MidiSwitch& operator=(MidiSwitch other);

    int index = 0;
    int midiNum = 0;


    float currentdB = -90;
    float certainty = 0;
    
    bool isOn = false;

    float retrigPct = 0;

    int onVel = 0;
    int onNote = 0;
};

inline float weightTodB(float weight, float refdB, float floor = -90)
{
    return floor + weight * (refdB - floor);
}

