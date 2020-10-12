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
};

struct MidiSwitch
{

    MidiSwitch(int index);

    void turnOn  (const MidiParams& params, const fvec& maxWeights);

    void update(const MidiParams& params, const fvec& weights);
    bool needsRelease (const MidiParams& params);

    void updateLowNote(int lowNote, const fvec& freqs, float refFreq);
    int calculateVelocity(const MidiParams& params);
    int midiShift(const MidiParams& params);

    static int getMidiNumber(float freq, float refFreq)
    {
        return int(0.5f + 69 + 12 * std::log2(freq / refFreq));
    }

    int index = 0;

    int lowestNote = 0;

    int midiNum = 0;

    float shortdB = 0;
    float longdB = 0;
    
    bool isOn = false;

    float retrigPct = 0;

    int onVel;
    int onNote;
};

