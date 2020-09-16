#include "Head.h"


enum class MidiState {off, init, on, retrigger};

struct MidiMode{};
struct ConstrainMidi : public MidiMode {};

struct MidiParams
{
    float note;
    float amp;
    float ampThresh;
    float releaseThresh;
    float retrig;
    float retrigStart;
    float retrigStop;
    float velDbMin;
    float velDbMax;
    int velMin;
    int velMax;
    int smoothFactor;
    bool constrain = false;
    int constrainStep = 2;
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
    bool equal(float note);
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
    SwitchMessage init  (const MidiParams& params);
    SwitchMessage on  (const MidiParams& params);
    SwitchMessage off (const MidiParams& params);
    SwitchMessage retrigger (const MidiParams& params);

    void onSequence(const MidiParams& p, SwitchMessage& m);
    void offSequence(const MidiParams& p, SwitchMessage& m);
    bool validAmp(const MidiParams& p);
    bool validWeight(const MidiParams& P);
    bool validRetrigger(const MidiParams& p);

    SwitchMessage changePitch();
    int getVelocity(const MidiParams& params);

    float smoothValue(const MidiParams& params);

    MidiState state = MidiState::off;
    
    Notes notes = Notes();
};

inline int midiRound(float val)
{
    return int(0.5 + val);
}

inline int getMidiNumber(float freq, float refFreq)
{
    return int(0.5f + 69 + 12 * std::log2(freq/refFreq));
}


inline float midiShift(int f0ind, const fvec& freqs, float refFreq, int octShift, int semitoneShift)
{
    if(f0ind == 0)
    {
        return 0;
    }
    
    int midi = getMidiNumber(freqs[f0ind], refFreq);
    int oct = octShift + midi/12;
    int pitch = (midi%12) + semitoneShift;

    int out = 12*oct + pitch;

    if(out < 0 || out > 127)
    {
        return 0;
    }
    
    return out;
}