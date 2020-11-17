
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

    dBSmoothing = blockSize/srate;

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



void Scribe::updateFundamental(const Range& range) 
{
    weights = dct(
        matrix, historyDS,
        range.lowNote, range.highNote,
        audio.ds.signalStart, historyDS.size());

    maxWeights = absMaxNormalize(weights, 0);
    

    //fundamental.index = maxArg(fundamentalCertainty);
    fundamental.prevIndex = fundamental.index;
    fundamental.index = maxArg(maxWeights);

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

    amp.dBPrev = amp.dB;
    amp.dB = int16ToDb(amp.val);
    amp.dB = (amp.dB < scribe.audio.mindB) || std::isnan(amp.dB) ? scribe.audio.mindB : amp.dB;

    

    blocks.dBShort = secToFraction(params.smoothTime.dBShort, scribe.audio.srate, scribe.audio.blockSize);
    blocks.dBLong  = secToFraction(params.smoothTime.dBLong, scribe.audio.srate, scribe.audio.blockSize);
    blocks.midi    = secToFraction(params.smoothTime.midi, scribe.audio.srate, scribe.audio.blockSize);

    delay.dBShort = SMABlocks(delay.dBShort, amp.dB, blocks.dBShort);
    delay.dBLong = SMABlocks(delay.dBLong, amp.dB, blocks.dBLong);
    consistency.current = float(scribe.fundamental.index == scribe.fundamental.prevIndex);
    consistency.history = SMABlocks(consistency.history, consistency.current, blocks.dBShort);

    amp.retrig =  delay.dBShort/delay.dBLong;
    //amp.retrig = amp.dB/delay.dBLong;
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

    output.weightVal = scribe.maxWeights[scribe.fundamental.index];
    output.weightThresh = 0;

    output.midiNum = midiShift(calcs.shift, scribe.midiNumbers[scribe.fundamental.index]);
    output.delaydB = calcs.amp.dB; //calcs.delay.dBShort;
    output.noiseThresh = calcs.threshold.noise;
    output.releaseThresh = calcs.threshold.release;

    output.retrigVal = calcs.amp.retrig;
    output.retrigStart = calcs.threshold.retrig;
    output.retrigStop = calcs.threshold.retrigStop;

    output.velocityVal = calcs.velocity.current;

    output.smoothFactor = calcs.blocks.midi;
    output.consistency = calcs.consistency.history;

    return output;
}
