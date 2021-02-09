
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

    history.reset(new Buffer<float>(audio.samples, 0.0f));

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



void Scribe::updateFundamental(const Range& range, const Blocks& blocks) 
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



    fundamental.prevIndex = fundamental.index;
    

    fundamental.index = maxArg(fundamentalCertainty);

    //pitch bending updates
    if(midiSwitch.state == MidiState::on && fundamental.lastActivated == 0)
    {
        fundamental.lastActivated = fundamental.prevIndex;
    }
    else if(midiSwitch.state != MidiState::on)
    {
        fundamental.lastActivated = 0;
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

    blocks.dBShort = secToFraction(params.smoothTime.dBShort, scribe.audio.srate, scribe.audio.blockSize);
    blocks.dBLong  = secToFraction(params.smoothTime.dBLong, scribe.audio.srate, scribe.audio.blockSize);
    blocks.midi    = secToFraction(params.smoothTime.midi, scribe.audio.srate, scribe.audio.blockSize);


    //center of mass w.r.t. signal y axis. Very similar to RMS, but with faster response;
    auto CoMs = CoMY3(scribe.historyDS);

    amp.val = SMABlocks(amp.val, CoMs[0], blocks.dBShort);
    amp.half1 = SMABlocks(amp.half1, CoMs[1], blocks.dBShort);
    amp.half2 = SMABlocks(amp.half2, CoMs[2], blocks.dBShort);

    amp.dBPrev = amp.dB;
    amp.dB = int16ToDb(amp.val);
    amp.dB = (amp.dB < scribe.audio.mindB) || std::isnan(amp.dB) ? scribe.audio.mindB : amp.dB;

    amp.retrig =  amp.half2/amp.val;
    

    //Threshold updates
    threshold = params.threshold;

    shift = params.shift;

    velocity = params.velocity;

    velocity.current = getVelocity(velocity, amp.dB, threshold.noise);

}

void Calculations::updateConsistency(const Scribe& scribe, const AudioParams& params)
{
    consistency.current = float(scribe.fundamental.index == scribe.fundamental.prevIndex);
    consistency.history = SMABlocks(consistency.history, consistency.current, blocks.dBShort);
    if(scribe.midiSwitch.state == MidiState::retrigger)
    {
        consistency.history = 0;
    }

    
    consistency.isConsistent =  consistency.history >= params.threshold.consistency? true : false;
    
}


void Calculations::updatePitchWheel(const Scribe& scribe, const AudioParams& params)
{
    int index = scribe.fundamental.lastActivated;
    float position = 0;

    if(index > 0 && scribe.midiSwitch.state == MidiState::on)
    {
        int low = params.range.lowNote;
        int high = params.range.highNote;

        int bendSemitones = 1; //can make this a user param in the future

        //get the indices surrounding the fundamental index;
        low  = index - bendSemitones > low  ? index - bendSemitones : low;
        high = index + bendSemitones < high ? index + bendSemitones : high;

        //add semitones to each side of the current index
        int weightRange = 2 * bendSemitones + 1;


        fvec weights = fvec(weightRange,0);

        for(int i = 0; i < high - low + 1; i++)
        {
            weights[i] = scribe.fundamentalCertainty[i + low];
        }

        absMaxNormalize(weights);

        for(int i = 0; i < weights.size(); i++)
        {
            weights[i] *= bendSemitones;
        }

        int centerInd = bendSemitones; //+1 wasn't needed hear due to 0 indexing.
        float centerVal = weights[centerInd];

        float left = maxArg(weights,0, centerInd);
        float right = maxArg(weights, centerInd + 1, weightRange);

        float bendVal = centerVal;
        float bendInd = centerInd;
        
        bendInd = weights[left] > weights[right] ? left : right;
        bendVal = weights[bendInd];
        
        
        if( bendVal > centerVal)
        {
            position = (bendVal - centerVal)/(bendInd - centerInd);
            //adjust to make the modulation more sensitive;
            //another potential user param
            float pitchScaling = 5;
            float sign = position < 0 ? -1 : 1;
            //scale the position based on frequency index--centered at index 45
            // -1.06 determined from solving for height difference between two different octaves
            position = pitchScaling * (position + sign * -.00416 * ( index - 45 ));

            //correct for excessive scaling;

            position = position > 1 ? 1 : position;
            position = position < -1 ? -1 : position;
            position = std::isnan(position) ? 0 : position;

            
            
            pitchWheelPosition = SMABlocks(pitchWheelPosition, position, blocks.dBLong);

            //std::cout <<centerVal << ", " <<bendVal << " | "<< centerInd << ", " << bendInd << " | " << position<< ", " << pitchWheelPosition << "\n";
        }
        else
        {
            pitchWheelPosition = 0;
        }

        
    }
    else
    {
        pitchWheelPosition = 0;
    }
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
    output.retrigSameStart = calcs.threshold.retrigSameNote;
    output.retrigStop = calcs.threshold.retrigStop;

    output.velocityVal = calcs.velocity.current;

    output.smoothFactor = calcs.blocks.midi;
    output.isConsistent = calcs.consistency.isConsistent;

    return output;
}
