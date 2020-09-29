#include "ProcessData.h"

// struct setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
Properties::Properties()
{
    freqSize = 1 + highExp - lowExp;
    histSamples = int(histTime * srate) + 1;

}

Properties::Properties(float srate, float blockSize)
{
    this->srate = srate;
    this->blockSize = blockSize;

    freqSize = 1 + highExp - lowExp;
    histSamples = int(histTime * srate) + 1;

    //dsRate = srate;
    dsFactor = srate/dsRate;
    dsHistSamples = int(histTime * dsRate);
    dsBlock = blockSize/dsFactor;
    
}

Storage::Storage()
{
}

Storage::Storage(Properties& props)
{
    int fSize = props.freqSize;

    //exFreq, exWeight, etc.
    //split each index into 3 components; ignore the last index
    int exSize = (fSize - 1) *3 + 1; 

    frequencies.reset   (new fvec(fSize,0));
    exFrequencies.reset (new fvec(exSize,0));
    
    history.reset   (new FloatBuffer(props.histSamples,0));
    historyDS.reset (new FloatBuffer(props.dsHistSamples,0));


    matrix.reset     ( new cmatrix (fSize,  cvec (props.dsHistSamples, std::complex<float>(0,0) )));
    exMatrix.reset   ( new cmatrix (exSize, cvec (props.dsHistSamples, std::complex<float>(0,0) )));

    timeVector.reset ( new fvec(props.dsHistSamples, 0));

    midiSwitch.reset ( new MidiSwitch());

    setETFrequencies (*frequencies.get(), props.refFreq,
                      props.semitone, props.lowExp, props.highExp);

    expandFrequencies (*frequencies.get(), *exFrequencies.get());

    setTimeVector (*timeVector.get(), props.dsRate);

    setComplexMatrix (*matrix.get(), *frequencies.get(), *timeVector.get());

    setComplexMatrix (*exMatrix.get(), *exFrequencies.get(), *timeVector.get());


}



void Calculations::updateRangeInfo(const AudioParams& params, int signalSize)
{
    loNote = params.loNote;
    hiNote = loNote + 48; //4 octaves

    //full signal start calculates octave 0 which isn't needed
    //half signal starts calculations on octave 1 which should be bass guitar range
    signalStart = 1 + signalSize / 2; 
}

void Calculations::updateSignalInfo(const fvec& weights, const fvec& ratios, const fvec& signal, const AudioParams& params)
{
    //center of mass w.r.t. signal y axis. Very similar to RMS, but with faster response;
    fvec comy3 = CoMY3(signal);

    //amplitude measured using the full signal window and half signal window
    ampFull = comy3[0];
    ampHalf1 = comy3[1];
    ampHalf2 = comy3[2];

    //fundamental index naively chosen by the greatest peak in frequency spectrum
    f0ind = maxArg(weights);
    f0ratio = ratios[f0ind];

    //max value associated with f0ind
    weight = weights[f0ind];
    retrigger = ampHalf2 / ampFull;

    /*float ref = notes.current * (1.0f - 1.0f/params.smoothFactor);
    ref += params.midiNum * 1.0f/params.smoothFactor;*/
    ampdB = int16ToDb(ampFull);
  
    //when attacksmooth is one attackdB is approximately ampdB
    attackdB =  attackdB * (1.0f - 1.0f / params.attackSmooth);
    attackdB += ampdB * 1.0f / params.attackSmooth;

    velocityAmp = velocityAmp * (1.0f - 1.0f / params.velocitySmooth);
    velocityAmp += velocityAmp * 1.0f / params.velocitySmooth;
    velocityAngle = std::atan(ampFull / velocityAmp);

    int loOct = loNote / 12;
    int currentOct = f0ind / 12;

    switch (currentOct - loOct) 
    {
        case 0:
            weightThreshold = params.weight0;
            noiseThreshold = params.noise0;
            break;
        case 1:
            weightThreshold = params.weight1;
            noiseThreshold = params.noise1;
            break;
        case 2:
            weightThreshold = params.weight2;
            noiseThreshold = params.noise2;
            break;
        case 3:
            weightThreshold = params.weight3;
            noiseThreshold = params.noise3;
            break;
        default:
            weightThreshold = params.weight3;
            noiseThreshold = params.noise3;
    }

    //note Ind undergoes an octave correction if necessary;
    //weight data can be distributed between 1st, 2nd and 3rd harmonics in some instances
    noteInd = f0ind;
    noteRatio = f0ratio;
    if (f0ratio < params.octStr)
    {
        noteInd = f0ind - 12;
        noteRatio = ratios[noteInd];
    }

    f0oct = f0ind / 12;
    f0pitch = f0ind % 12;

    noteOct = noteInd / 12;
    notePitch = noteInd % 12;
}

void Calculations::updateMidiNum(const Storage& storage, const Properties& props, const AudioParams& params, const Calculations& calcs) 
{
    int ind = calcs.noteInd;
    ind = weight < weightThreshold ? 0 : ind;
    ind = attackdB < noiseThreshold ? 0 : ind;
    ind = ind < 0 ? 0 : ind;

    midiNum = midiShift(ind, *storage.frequencies.get(), props.refFreq, params.octave, params.semitone);
}

fvec getPeaks(const fvec& signal)
{

    fvec peaks(signal.size(),0);
    for(int i = 1; i < signal.size()-1; i++)
    {
        peaks[i] = (signal[i -1] < signal[i]) && (signal[i] > signal[i+1]) ? 1 : 0;
    }
    
    peaks[0] = signal[0] > signal[1] ? 1 : 0;

    int last = signal.size()-1;
    peaks[last] = signal[last] > signal[last -1] ? 1 : 0;
    return peaks;
}

void alignWithPeaks(fvec& arr, const fvec& peaks)
{
    for(int i = 0; i < arr.size(); i++)
    {
        arr[i] = arr[i] * peaks[i];
    }
}


void clearBelowInd(fvec& arr, int ind)
{
    for(int i = 0; i < ind; i++)
    {
        arr[i] = 0;
    }
}

void clearAboveInd(fvec& arr, int ind)
{
    for(int i = ind + 1; i < arr.size(); i++)
    {
        arr[i] = 0;
    }
}

fvec weightRatio(const fvec& arr, int octSize)
{
    fvec output(arr.size(), 0);

    for(int i = octSize; i < arr.size(); i++)
    {
        output[i] = arr[i] /arr[i-octSize];
        output[i] = output[i] > 10 ? 10 : output[i];
    }

    return output;
}



MidiParams getMidiParams(const Calculations& calcs, const AudioParams& params)
{
    auto output = MidiParams();

    output.weight = calcs.weight;
    output.weightThreshold = calcs.weightThreshold;

    output.midiNum = calcs.midiNum;
    output.ampdB = calcs.attackdB;
    output.noisedB = calcs.noiseThreshold;
    output.releasedB = params.release;

    output.retrig = calcs.retrigger;
    output.retrigStart = params.retrigStart;
    output.retrigStop = params.retrigStop;
    
    output.velPTheta = params.velPTheta;
    output.velocityAngle = calcs.velocityAngle;
    output.velMin = params.velMin;
    output.velMax = params.velMax;
    
    output.smoothFactor = params.midiSmooth;

    return output;
}