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
    loNote = params.loOct * 12;
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

    ampdB = int16ToDb(ampFull);

    //note validation --sending to 0 will cue MidiSwitch to ignore this note
    f0ind = weight < params.weightThreshold ? 0 : f0ind;
    f0ind = f0ind < loNote ? 0 : f0ind;
    f0ind = ampdB < params.noise ? 0 : f0ind;

    //note Ind undergoes an octave correction if necessary;
    //weight data can be distributed between 1st, 2nd and 3rd harmonics in some instances
    noteInd = f0ind;
    noteRatio = f0ratio;
    if (f0ratio < params.octStr)
    {
        noteInd = f0ind - 12 < 0 ? 0 : f0ind - 12;
        noteRatio = ratios[noteInd];
        weight = weights[noteInd] + weights[f0ind];
    }

    f0oct = f0ind / 12;
    f0pitch = f0ind % 12;

    noteOct = noteInd / 12;
    notePitch = noteInd % 12;
}

void Calculations::updateMidiNum(const Storage& storage, const Properties& props, const AudioParams params) 
{
    midiNum = midiShift(noteInd, *storage.frequencies.get(), props.refFreq, params.octave, params.semitone);
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
    }

    return output;
}


MidiParams getMidiParams(const Calculations& calcs, const AudioParams& params)
{
    auto output = MidiParams();

    output.weight = calcs.weight;
    output.weightThreshold = params.weightThreshold;

    output.midiNum = calcs.midiNum;
    output.ampdB = calcs.ampdB;
    output.noisedB = params.noise;
    output.releasedB = params.release;

    output.retrig = calcs.retrigger;
    output.retrigStart = params.retrigStart;
    output.retrigStop = params.retrigStop;
    
    output.velDbMin = params.velDbMin;
    output.velDbMax = params.velDbMax;
    output.velMin = params.velMin;
    output.velMax = params.velMax;
    
    output.smoothFactor = params.smooth;

    return output;
}