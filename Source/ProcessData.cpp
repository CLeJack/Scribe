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

// fill process data vectors ~~~~~~~~~~~~~~~~~~~~~`




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


