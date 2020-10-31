
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

    setOctErrMatrix1(maxOctMatrix, matrix, timeVector, frequencies, absMaxNormalize );
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



void Scribe::updateFundamental(const Range& range, const Blocks& blocks, const Amp& amp, const Threshold& thresh) 
{
    weights = dct(
        matrix, historyDS,
        range.lowNote, range.highNote,
        audio.ds.signalStart, historyDS.size());

    maxWeights = absMaxNormalize(weights, 0);
    sumWeights = sumNormalize(weights, 0);

    fmatrix freqMatrix = freqCertaintyMatrix(
        maxWeights, maxOctMatrix,
        range.lowNote, range.highNote,
        0, weights.size());

    fundamentalCertainty = freqCertaintyVector(
        sumWeights, freqMatrix,
        range.lowNote, range.highNote,
        0, weights.size());

    //clear info that is out of bounds in case the range moves up and down
    for (int i = 0; i < range.lowNote; i++) 
    {
        fundamentalHistory[i] = 0;
    }

    for (int i = range.highNote; i < fundamentalHistory.size(); i++) 
    {
        fundamentalHistory[i] = 0;
    }


    //get history info in range
    for(int i = range.lowNote; i < range.highNote; i++)
    {
        fundamentalHistory[i] = SMA(fundamentalHistory[i], fundamentalCertainty[i], blocks.midi);
    }

    fundamental.prevIndex = fundamental.index;

    
    //peaks = fvec(fundamentalHistory.size(), 0);
    if (amp.dB > thresh.noise) 
    {
        fundamental.index = maxArg(fundamentalHistory);
        //fundamental.index = maxArg(fundamentalCertainty);
        //peaks[fundamental.index] = 1;
    }
    

}

void Scribe::updateChords(const Range& range, const Blocks& blocks, const Amp& amp, const Threshold& thresh) 
{

    if(fOnNotes[fundamental.index])
    {
        fmatrix chordMatrix = chordCertaintyMatrix(
            fundamental.index, tuning.octaveSize, 
            maxWeights, maxSineMatrix, 
            0, weights.size());
        
        chordCertainty = freqCertaintyVector(
            sumWeights, chordMatrix, 
            range.lowNote, range.highNote, 
            0, weights.size());
        
        for (int i = 0; i < fundamental.index; i++) 
        {
            chordHistory[i] = 0;
        }

        for (int i = range.highNote; i < chordHistory.size(); i++) 
        {
            chordHistory[i] = 0;
        }

        for(int i = fundamental.index; i < range.highNote; i++)
        {
            chordHistory[i] = SMA(chordHistory[i], chordCertainty[i], blocks.midi);
        }

    
        peaks = getPeaks(chordHistory);
        chordAvg = positiveMean(chordHistory, fundamental.index, range.highNote);
        float chordMax = maxValue(chordHistory, fundamental.index, range.highNote);
        peakFloor = thresh.chordPct * (chordMax - chordAvg) + chordAvg;
        int peakSum = sum(peaks);

        //I don't want to include the fundamental in the chord data
        //because I don't want it to be turned off within the chord update logic
        //the note next to the fundamental is removed due to the simplicty of the peak algo
        //corners will very frequently be considered peaks
        //This could be resolved by increasing the number of frequency bins to 3 per note
        //and increasing the octave size to 36, but I don't want to mess with this right now
        peaks[fundamental.index] = 0;
        peaks[fundamental.index + 1] = 0;
        for(int i = fundamental.index; i < range.highNote; i++)
        {
            peaks[i] = chordHistory[i] < peakFloor ? 0 : peaks[i];
        }
    }
    else
    {
        peaks = fvec(chordHistory.size(), 0);
    }

    //This +2 offset is needed due ot the
    for(int i = fundamental.index; i < range.highNote; i++)
    {
        peaksHistory[i] = SMA(peaksHistory[i], peaks[i], blocks.midi);
    }
    
    
}
void Scribe::updatePeaks() {}

void Scribe::updateFMidiInfo(
    const Threshold& thresh, const Amp& amp, const Velocity& vel, 
    const Range& range, const Shift& shift, const Blocks& blocks)
{
    //runChords = !runChords; //force next cycle to switch between updateFundamental and updateChords

    float certainty_level = .6; //temporary until I add adjustments back or work out a formula;
    if (
            fOnNotes[fundamental.index] == false 
            && amp.dB > thresh.noise 
            && amp.retrig >= thresh.retrig
            && fundamentalHistory[fundamental.index] > certainty_level
            //&& std::abs(amp.slope) <= thresh.slope
            //a good trigger point as been found
       )
    {
        fNeedsTrigger[fundamental.index] = true;
        fNeedsRelease[fundamental.index] = false;
        inTriggerState = true;

        finalNote[fundamental.index] = midiShift(shift, midiNumbers[fundamental.index]);

    }

    for(int i = 0; i < fNeedsRelease.size(); i++)
    {

        if(fOnNotes[i] == true )
        {
            if(
                amp.dB < thresh.release 
                || i < range.lowNote 
                || i >= range.highNote
                //|| (amp.retrig < thresh.retrig && inTriggerState == false)
                || (amp.retrig < thresh.retrig)
                //||std::abs(amp.slope) <= thresh.slope && inTriggerState == false
                || i != fundamental.index
                 )
            {
                fNeedsRelease[i] = true;
                fNeedsTrigger[i] = false;
            }
        } 
    }
    
    if(amp.retrig >= thresh.retrig && fNeedsTrigger[fundamental.index] == false)
    {
        inTriggerState = false;
    }
}

void Scribe::updateCMidiInfo(
    const Threshold& thresh, const Amp& amp, const Velocity& vel,
    const Range& range, const Shift& shift) 
{
    for(int i = 0; i < cNeedsRelease.size(); i++)
    {
        if (peaksHistory[i] >= .5 && cOnNotes[i] == false && amp.dB > thresh.noise)
        {
            cNeedsTrigger[i] = true;
            cNeedsRelease[i] = false;

            finalNote[i] = midiShift(shift, midiNumbers[fundamental.index]);
        }

        if(cOnNotes[i] == true && i != fundamental.index )
        {
            if(
                amp.dB < thresh.release 
                || i < range.lowNote 
                || i >= range.highNote
                || (amp.retrig < thresh.retrig && inTriggerState == false)
                || peaks[i] != 1
                 )
            {
                fNeedsRelease[i] = true;
                fNeedsTrigger[i] = false;
            }
        } 
    }

}

void Scribe::turnOnMidi(int i, const Amp& amp, const Threshold& thresh) 
{
    if (fNeedsTrigger[i]) 
    {
        fOnNotes[i] = true;
        fNeedsTrigger[i] = false;
    }
    /*
    if(cNeedsTrigger[i])
    {
        cOnNotes[i] = true;
        cNeedsTrigger[i] = false;
    }
    */
}

void Scribe::turnOffMidi(int i)
{

    if(fNeedsRelease[i])
    {
        fNeedsRelease[i] = false;
        fOnNotes[i] = false;
        fNeedsTrigger[i] = false;
        finalNote[i] = midiNumbers[i];
    }
    if (i == fundamental.index) {
        inTriggerState = false;
    }
    /*
    if(cNeedsRelease[i])
    {
        cNeedsRelease[i] = false;
        cOnNotes[i] = false;
        cNeedsTrigger[i] = false;
        finalNote[i] = midiNumbers[i];
    }
    */
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
    amp.dB = (amp.dB < scribe.audio.mindB) || std::isnan(amp.dB) ? scribe.audio.mindB : amp.dB;

    blocks.dBShort = secToBlocks (params.smoothTime.dBShort, scribe.audio.srate, scribe.audio.blockSize);
    blocks.dBLong = secToBlocks (params.smoothTime.dBLong, scribe.audio.srate, scribe.audio.blockSize);
    blocks.midi = secToBlocks (params.smoothTime.midi, scribe.audio.srate, scribe.audio.blockSize);
    blocks.midiDelay = secToBlocks(params.threshold.midiDelay, scribe.audio.srate, scribe.audio.blockSize);

    delay.dBShort = SMA(delay.dBShort, amp.dB, blocks.dBShort);
    delay.dBLong = SMA(delay.dBLong, amp.dB, blocks.dBLong);

    //amp.retrig =  delay.dBShort/delay.dBLong;
    amp.retrig = amp.dB/delay.dBShort;
    amp.slope = (delay.dBShort - delay.dBLong)
              / std::abs(params.smoothTime.dBShort - params.smoothTime.dBLong);
    

    //Threshold updates
    threshold = params.threshold;

    shift = params.shift;

    velocity = params.velocity;

    velocity.current = getVelocity(velocity, amp.dB, threshold.noise);

}

MidiParams getMidiParams(const Calculations& calcs, Scribe& scribe)
{

    auto output = MidiParams();

    //All of these values should be updated in Calculations::updateSignalCalcs()

    output.weightVal = scribe.fundamentalCertainty[scribe.fundamental.index];
    output.weightThresh = 0;

    output.midiNum = midiShift(calcs.shift, scribe.midiNumbers[scribe.fundamental.index]);
    output.delaydB = calcs.amp.dB; //calcs.delay.dBShort;
    output.noiseThresh = calcs.threshold.noise;
    output.releaseThresh = calcs.threshold.release;

    output.retrigVal = calcs.amp.retrig;
    output.retrigStart = calcs.threshold.retrig;
    output.retrigStop = calcs.threshold.retrigStop;

    output.velocityVal = calcs.velocity.current;

    output.smoothFactor = calcs.blocks.dBShort;

    return output;
}
