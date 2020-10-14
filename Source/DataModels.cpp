
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

    for(int i = 0; i < frequencies.size(); i++)
    {
        midiNumbers[i] = getMidiNumber(frequencies[i], tuning.refFreq);
    }

    setTimeVector(timeVector, audio.ds.srate);

    setComplexMatrix(matrix, frequencies, timeVector);

    setSineMatrix(sumSineMatrix, matrix, timeVector, frequencies, sumNormalize );
    setSineMatrix(maxSineMatrix, matrix, timeVector, frequencies, absMaxNormalize);

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

void Scribe::updateWeights(int lowNote, int highNote, int midiBlocks, float dB)
{
    weights = dct(
        matrix, historyDS,
        lowNote, highNote, 
        audio.ds.signalStart, historyDS.size());

    maxWeights = absMaxNormalize(weights, 0);

    for(int i = 0; i < maxWeights.size(); i++)
    {
        maxWHistory[i] = SMA(maxWHistory[i], maxWeights[i], midiBlocks);
        notedB[i] = maxWHistory[i] * (dB + 90) - 90; //90 = good approx to 0 for dB scale
    }

    peaks = getPeaks(maxWHistory);
}

void Scribe::updateMidiInfo(
    const Threshold& thresh, const Amp& amp, const Velocity& vel, 
    const Range& range, const Shift& shift)
{
    for(int i = 0; i < onNotes.size(); i++)
    {
        if(maxWHistory[i] > thresh.certainty && peaks[i] == 1)
        {
            onNotes[i] = true;
            noteVel[i] = getVelocity(vel, amp.dB, thresh.release);
            finalNote[i] = midiShift(shift, midiNumbers[i]);
        }

        if(onNotes[i] == true && 
        (notedB[i] < thresh.release 
        || i < range.lowNote 
        || i >= range.highNote)
        || maxWHistory[i] < thresh.certainty )
        {
            needsRelease[i] = true;
        }
    }
}

void Scribe::turnOffMidi(int i)
{

    if(needsRelease[i])
    {
        needsRelease[i] = false;
        onNotes[i] = false;
        noteVel[i] = 0;
        finalNote[i] = midiNumbers[i];
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void Calculations::updateRange(const Scribe& scribe, const AudioParams& params)
{
    range.lowNote = params.range.lowNote;
    range.highNote = range.lowNote + 4 * scribe.tuning.octaveSize;
}


void Calculations::updateSignal(const Scribe& scribe, const AudioParams& params)
{

    //center of mass w.r.t. signal y axis. Very similar to RMS, but with faster response;
    amp.val = CoMY(scribe.historyDS);

    amp.dB = int16ToDb(amp.val);
    amp.dB = (amp.dB < -90) || std::isnan(amp.dB) ? -90 : amp.dB;

    blocks.dBShort = secToBlocks (params.smoothTime.dBShort, scribe.audio.srate, scribe.audio.blockSize);
    blocks.dBLong = secToBlocks (params.smoothTime.dBLong, scribe.audio.srate, scribe.audio.blockSize);
    blocks.midi = secToBlocks (params.smoothTime.midi, scribe.audio.srate, scribe.audio.blockSize);

    delay.dBShort = SMA(delay.dBShort, amp.dB, blocks.dBShort);
    delay.dBLong = SMA(delay.dBLong, amp.dB, blocks.dBLong);

    //Threshold updates
    threshold.release     = params.threshold.release;
    threshold.retrigStart = params.threshold.retrigStart;
    threshold.retrigStop  = params.threshold.retrigStop;
    threshold.certainty   = params.threshold.certainty;

    shift.octave   = params.shift.octave;
    shift.semitone = params.shift.semitone;

    velocity.max   = params.velocity.max;
    velocity.min   = params.velocity.min;
    velocity.maxdB = params.velocity.maxdB;

}


MidiParams getMidiParams(const Calculations& calcs)
{

    auto output = MidiParams();

    //All of these values should be updated in Calculations::updateSignalCalcs()
    //This is redundant, but it ensures that every value has gone through some
    //sort of processing--that I'm not just pulling from a default;

    //~~~~~~~~~~~~~~~~~~~
    output.certaintyThresh = calcs.threshold.certainty;
    output.releaseThresh   = calcs.threshold.release;

    output.retrigStart = calcs.threshold.retrigStart;
    output.retrigStop = calcs.threshold.retrigStop;
    
    output.refdB = calcs.amp.dB;
    
    output.smoothingBlocks = calcs.blocks.midi;

    output.octShift  = calcs.shift.octave;
    output.semiShift = calcs.shift.octave;

    output.maxdB  = calcs.velocity.maxdB;
    output.maxVel = calcs.velocity.max;
    output.minVel = calcs.velocity.min;

    output.lowNote = calcs.range.lowNote;
    output.highNote = calcs.range.highNote;

    return output;
}