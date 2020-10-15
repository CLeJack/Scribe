
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

void Scribe::updateWeights(int lowNote, int highNote, int midiBlocks, const Amp& amp, const Threshold& thresh)
{
    weights = dct(
        matrix, historyDS,
        lowNote, highNote, 
        audio.ds.signalStart, historyDS.size());

    maxWeights = absMaxNormalize(weights, 0);
    sumWeights = sumNormalize(weights, 0);

    fmatrix freqMatrix = freqCertaintyMatrix(
        maxWeights, maxOctMatrix,
        lowNote, highNote,
        0, weights.size());

    certainty = freqCertaintyVector(
        sumWeights, freqMatrix,
        lowNote, highNote,
        0, weights.size());

    

    for(int i = lowNote; i < highNote; i++)
    {
        weightHistory[i] = SMA(weightHistory[i], certainty[i], midiBlocks);
    }


    
    
    fmatrix chordMatrix = chordCertaintyMatrix(
        fundamental.index, tuning.octaveSize, 
        maxWeights, maxSineMatrix, 
        0, weights.size());
    
    chordCertainty = freqCertaintyVector(
        sumWeights, chordMatrix, 
        lowNote, highNote, 
        0, weights.size());
    
    
    for(int i = lowNote; i < highNote; i++)
    {
        chordHistory[i] = SMA(chordHistory[i], chordCertainty[i], midiBlocks);
    }
    

    peaks = fvec(weightHistory.size(), 0);
    if (amp.dB > thresh.noise) 
    {
        //peaks = getPeaks(chordHistory);
        peaks = getPeaks(weightHistory);
        fundamental.history = fundamental.index;
        fundamental.index = maxArg(weightHistory);
        peaks[fundamental.index] = 1;
        //chordAvg = positiveMean(chordHistory, fundamental.index, highNote);
        chordAvg = positiveMean(weightHistory, fundamental.index, highNote);
        
        for(int i = lowNote; i < highNote; i++)
        {
            if(fundamental.history != 0 && fundamental.history == fundamental.index)
            {
                //peakFloor = thresh.chordPct * (chordHistory[fundamental.index] - chordAvg) + chordAvg;
                peakFloor = thresh.chordPct * (weightHistory[fundamental.index] - chordAvg) + chordAvg;
            }
            //peaks[i] = chordHistory[i] < peakFloor ? 0 : peaks[i];
            peaks[i] = weightHistory[i] < peakFloor ? 0 : peaks[i];
            peaksHistory[i] = SMA(peaksHistory[i], peaks[i], 4.0f);
        }

        
        
    }

}

void Scribe::updateMidiInfo(
    const Threshold& thresh, const Amp& amp, const Velocity& vel, 
    const Range& range, const Shift& shift)
{
    for(int i = 0; i < needsTrigger.size(); i++)
    {
        //if(weightHistory[i] > thresh.certainty && peaks[i] == 1 && onNotes[i] == false)
        //if (peaks[i] == 1 && onNotes[i] == false && amp.dB > thresh.noise)
        if (peaksHistory[i] >0.5f && onNotes[i] == false && amp.dB > thresh.noise)
        {
            needsTrigger[i] = true;
            
            finalNote[i] = midiShift(shift, midiNumbers[i]);
        }

        if(onNotes[i] == true )
        {
            if(
                amp.dB < thresh.release 
                || i < range.lowNote 
                || i >= range.highNote
                || amp.retrig < thresh.retrig && needsTrigger[i] == false
                //|| i == fundamental.history && fundamental.history != fundamental.index
                //|| i != fundamental.index && chordHistory[i] < thresh.chordPct * chordHistory[fundamental.index]
                //|| weightHistory[i] < thresh.chordPct * weightHistory[fundamental.index]
                || peaksHistory[i] < 0.5f
                 )
            {
                needsRelease[i] = true;
            }

            /*
            if(i == fundamental.history )
            {
                needsRelease[fundamental.history] = true;
            }*/
        } 
        
    }
}

void Scribe::turnOnMidi(int i, const Amp& amp, const Threshold& thresh) 
{
    if (needsTrigger[i]) 
    {
        onNotes[i] = true;
        if(amp.retrig > thresh.retrig)
        {
            //wait for retrigger phase to stop before deciding to
            //turn off trigger flag.
            needsTrigger[i] = false;
        }
    }
}

void Scribe::turnOffMidi(int i)
{

    if(needsRelease[i])
    {
        needsRelease[i] = false;
        onNotes[i] = false;
        needsTrigger[i] = false;
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
    amp.dB = (amp.dB < scribe.audio.mindB) || std::isnan(amp.dB) ? scribe.audio.mindB : amp.dB;

    blocks.dBShort = secToBlocks (params.smoothTime.dBShort, scribe.audio.srate, scribe.audio.blockSize);
    blocks.dBLong = secToBlocks (params.smoothTime.dBLong, scribe.audio.srate, scribe.audio.blockSize);
    blocks.midi = secToBlocks (params.smoothTime.midi, scribe.audio.srate, scribe.audio.blockSize);

    delay.dBShort = SMA(delay.dBShort, amp.dB, blocks.dBShort);
    delay.dBLong = SMA(delay.dBLong, amp.dB, blocks.dBLong);

    amp.retrig = delay.dBShort/delay.dBLong;
    

    //Threshold updates
    threshold = params.threshold;

    shift = params.shift;

    velocity = params.velocity;

}


void Calculations::updateFundamental(const Scribe& scribe)
{

    if(amp.retrig < threshold.retrig && amp.dB > threshold.noise)
    {
        fundamental.history = fundamental.index;
        fundamental.index = maxArg(scribe.weightHistory); 
    }

    if(amp.retrig > threshold.retrig && fundamental.history != fundamental.index)
    {
        fundamental.history = fundamental.index;
    }

    if(amp.dB < threshold.release)
    {
        fundamental.index = 0;
        fundamental.history = 0;
    }
}