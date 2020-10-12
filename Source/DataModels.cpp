
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

    setSineMatrix(sumSineMatrix, matrix, timeVector, frequencies, sumNormalize );
    setSineMatrix(maxSineMatrix, matrix, timeVector, frequencies, absMaxNormalize);

    //setOctErrMatrix1(sumOctErrMatrix1, matrix, timeVector, frequencies, sumNormalize);
    //setOctErrMatrix1(maxOctErrMatrix1, matrix, timeVector, frequencies, absMaxNormalize);

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

void Scribe::updateWeights(int lowNote, int highNote)
{
    weights = dct(matrix, historyDS,
            lowNote, highNote, 
            audio.ds.signalStart, historyDS.size());

        maxWeights = absMaxNormalize(weights, 0);
        sumWeights = sumNormalize(weights, 0);

        fmatrix freqMatrix = freqCertaintyMatrix(maxWeights, maxSineMatrix,
        lowNote, highNote, 
        0, weights.size());

        fvec certainty = freqCertaintyVector(sumWeights, freqMatrix, 
        lowNote, highNote,
         0, weights.size());
}

void Scribe::updateCertaintyPeaks(float certaintyThreshold)
{
    fvec unitPeaks = getPeaks(certainty);

    for (int i = 0; i < peaks.size() ; i++)
    {
        peaks[i] = unitPeaks[i] == 1 ? certainty[i] : 0;
    }
}


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

    return output;
}



void updateMidi(Scribe& scribe, const Calculations& calcs, const MidiParams& midiParams)
{

    for(int i = 0; i < scribe.midiPanel.size(); i++)
    {
        //updates lowestNote for midiswitch if there has been a change
        scribe.midiPanel[i].updateLowNote(calcs.range.lowNote, 
                                          scribe.frequencies, scribe.tuning.refFreq);

        
        if(scribe.peaks[i] == 1 && scribe.midiPanel[i].isOn == false)
        {
            scribe.midiPanel[i].turnOn(midiParams, scribe.maxWeights);
        }

        //updates dB only at the moment
        scribe.midiPanel[i].update(midiParams, scribe.maxWeights);

        //turnoff behaviors happens internal to midiswitch and sets a flag
        //midi off behavior will happen in the process block
    }
}