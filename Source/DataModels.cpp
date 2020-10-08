
#include "DataModels.h"

void Scribe::initialize(float srate, float blockSize)
{

    audio.srate = srate;
    audio.blockSize = blockSize;
    audio.samples = audio.historyTime() * audio.srate + 1;

    audio.ds.factor = audio.srate / audio.ds.srate;
    audio.ds.blockSize = audio.blockSize / audio.ds.factor;


    setETFrequencies(
        frequencies,
        tuning.refFreq,
        tuning.semitone,
        tuning.lowExp,
        tuning.highExp);

    setTimeVector(timeVector, audio.ds.srate);

    setComplexMatrix(matrix, frequencies, timeVector);

    history.reset(new FloatBuffer(audio.samples, 0.0f));

    isInitialized = true;
}

bool Scribe::detectsPropertyChange(float srate, float blockSize) 
{
    if (audio.srate != srate || audio.blockSize != blockSize) 
    {
        isInitialized = false;
    }
    return isInitialized;
}

void Calculations::updateRange(const Range& params)
{
    range.lowNote = params.lowNote;
    range.highNote = range.lowNote + 48; //4 octaves
}


void Calculations::updateSignal(const Scribe& scribe, const AudioParams& params)
{

    //center of mass w.r.t. signal y axis. Very similar to RMS, but with faster response;
    fvec comy2 = CoMY2(scribe.historyDS);

    amp.val = comy2[0];
    amp.half1 = 0; // oldest half of the signal
    amp.half2 = comy2[1]; // newest half

    //fundamental index chosen by the greatest peak in frequency spectrum
    fundamental.index = maxArg(scribe.weights);
    fundamental.ratio = scribe.ratios[fundamental.index];

    targets.weight = scribe.weights[fundamental.index];
    targets.retrigger = amp.half2 / amp.val; //compare the full signal amplitude with the earliest half

    amp.dB = int16ToDb(amp.val);
    amp.dB = (amp.dB < -90) || std::isnan(amp.dB) ? -90 : amp.dB;

    blocks.dBShort = secToBlocks (params.smoothTime.dBShort, scribe.audio.srate, scribe.audio.blockSize);
    blocks.dBLong = secToBlocks (params.smoothTime.dBLong, scribe.audio.srate, scribe.audio.blockSize);
    blocks.midi = secToBlocks (params.smoothTime.midi, scribe.audio.srate, scribe.audio.blockSize);

    delay.dBShort = SMA(delay.dBShort, amp.dB, blocks.dBShort);
    delay.dBLong = SMA(delay.dBLong, amp.dB, blocks.dBLong);

    //do velocity calc here


    // note Ind undergoes an octave correction if necessary;
    // weight data can be distributed between 1st, 2nd and 3rd harmonics in some instances
    // this is the only way I can see to fix this until I implement statistical methods
    note.index = fundamental.index;
    note.ratio = fundamental.ratio;
    if (fundamental.ratio < params.threshold.ratio
        && fundamental.index >= scribe.tuning.octaveSize)
    {
        note.index -= scribe.tuning.octaveSize;
        note.ratio = scribe.ratios[note.index];
    }

    //Threshold updates
    int loOct = range.lowNote / scribe.tuning.octaveSize;
    int currentOct = fundamental.index / scribe.tuning.octaveSize;

    switch (currentOct - loOct)
    {
    case 0:
        threshold.weight = params.threshold.weight0;
        threshold.noise = params.threshold.noise0;
        break;
    case 1:
        threshold.weight = params.threshold.weight1;
        threshold.noise = params.threshold.noise1;
        break;
    case 2:
        threshold.weight = params.threshold.weight2;
        threshold.noise = params.threshold.noise2;
        break;
    case 3:
        threshold.weight = params.threshold.weight3;
        threshold.noise = params.threshold.noise3;
        break;
    default:
        threshold.weight = params.threshold.weight3;
        threshold.noise = params.threshold.noise3;
    }

    threshold.release = params.threshold.release;
    threshold.retrigStart = params.threshold.retrigStart;
    threshold.retrigStop = params.threshold.retrigStop;


    fundamental.octave /= scribe.tuning.octaveSize;
    fundamental.pitch %= scribe.tuning.octaveSize;

    note.octave /= scribe.tuning.octaveSize;
    note.pitch %= scribe.tuning.octaveSize;
}


void Calculations::updateMidi(const Scribe& scribe, const AudioParams& params)
{
    int ind = note.index;
    ind = targets.weight < threshold.weight ? 0 : ind;
    ind = delay.dBShort < threshold.noise ? 0 : ind;
    ind = ind < 0 ? 0 : ind;

    midi.index = midiShift(ind, scribe.frequencies, scribe.tuning.refFreq, params.shift.octave, params.shift.semitone);
    midi.velocity = midiVelocity(params.velocity.max, params.velocity.min, delay.dBShort, delay.dBLong, params.velocity.ratio);
}


MidiParams getMidiParams(const Calculations& calcs)
{

    auto output = MidiParams();

    //All of these values should be updated in Calculations::updateSignalCalcs()

    output.weightVal = calcs.targets.weight;
    output.weightThresh = calcs.threshold.weight; 

    output.midiNum = calcs.midi.index;
    output.delaydB = calcs.delay.dBShort;
    output.noiseThresh = calcs.threshold.noise;
    output.releaseThresh = calcs.threshold.release;

    output.retrigVal = calcs.targets.retrigger;
    output.retrigStart = calcs.threshold.retrigStart;
    output.retrigStop = calcs.threshold.retrigStop;

    output.velocityVal = calcs.midi.velocity;

    output.smoothFactor = calcs.blocks.midi;

    return output;
}