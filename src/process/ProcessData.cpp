#pragma once
#include "ProcessData.h"

// struct setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
ProcessProperties::ProcessProperties()
{
    freqSize = 1 + highExp - lowExp;
    histSamples = int(histTime * srate) + 1;

}

ProcessProperties::ProcessProperties(float srate, float blockSize)
{
    this->srate = srate;
    this->blockSize = blockSize;

    freqSize = 1 + highExp - lowExp;
    histSamples = int(histTime * srate) + 1;
    transBlocks = int(histSamples/blockSize)+1;

    //dsRate = srate;
    dsFactor = srate/dsRate;
    dsHistSamples = int(histTime * dsRate);
    dsBlock = blockSize/dsFactor;
    
}

ProcessData::ProcessData()
{
}

ProcessData::ProcessData(ProcessProperties& props)
{
    int fSize = props.freqSize;

    //exFreq, exWeight, etc.
    //split each index into 3 components; ignore the last index
    int exSize = (fSize - 1) *3 + 1; 

    frequencies.reset   (new fvec(fSize,0));
    exFrequencies.reset (new fvec(exSize,0));
    midiNumbers.reset   (new fvec(fSize,0));
    
    history.reset   (new FloatBuffer(props.histSamples,0));
    historyDS.reset (new FloatBuffer(props.dsHistSamples,0));

    noteLimits.reset ( new fvec(fSize, 1));

    matrix.reset     ( new cmatrix (fSize,  cvec (props.dsHistSamples, std::complex<float>(0,0) )));
    exMatrix.reset   ( new cmatrix (exSize, cvec (props.dsHistSamples, std::complex<float>(0,0) )));

    timeVector.reset ( new fvec(props.dsHistSamples, 0));

    kernels.reset       ( new fmatrix(fSize, fvec(props.dsHistSamples,0)));
    kernelWeights.reset ( new fmatrix(fSize, fvec(fSize, 0)));

    midiSwitch.reset ( new MidiSwitch());

    setETFrequencies (*frequencies.get(), props.refFreq,
                      props.semitone, props.lowExp, props.highExp);

    expandFrequencies (*frequencies.get(), *exFrequencies.get());
    
    setMidiNumbers (*midiNumbers.get(), *frequencies.get(), props.refFreq);

    setTimeVector (*timeVector.get(), props.dsRate);

    setParabolicKernels(*kernels.get(), *frequencies.get(), props.dsRate);

    setBlockLimits(*noteLimits.get(), *frequencies.get(), props.srate, props.blockSize, props.transBlocks);

    setComplexMatrix (*matrix.get(), *frequencies.get(), *timeVector.get());
    //setCSquareMatrix (*matrix.get(), *frequencies.get(), *timeVector.get());

    setComplexMatrix (*exMatrix.get(), *exFrequencies.get(), *timeVector.get());

    setParabolicWeights(*kernelWeights.get(), *kernels.get(), *matrix.get());

}

// fill process data vectors ~~~~~~~~~~~~~~~~~~~~~`






void setSineWeights(fmatrix& weights, const cmatrix& matrix, const fvec& freqs, const fvec& timeVector, int clearLast)
{
    fvec waveform;
    fvec weight;
    for(int row = 0; row < freqs.size(); row++)
    {
        waveform = sinusoid(timeVector, freqs[row], 0, 1);
        weights[row] = sumNormalize(dct(matrix, waveform));
        clearAboveInd(weights[row], freqs.size() - clearLast);
    }
}



void setTimeVector(fvec& timeVector, float srate)
{
    for(int i = 0; i < timeVector.size(); i++)
    {
        // 1 / srate = time increment
        timeVector[i] = (i * (1.0 / srate) );
    }
}

void setParabolicKernels(fmatrix& kernels, const fvec& freqs, float srate)
{
    int window = 0;

    for(int row = 0; row < kernels.size(); row++)
    {
        window = freqWindow(freqs[row], srate);
        fvec values = parabolicKernel(window);

        for(int i = 0; i < window; i++)
        {

            kernels[row] = values;
        }
        
    }
}

void setParabolicWeights(fmatrix & pWeights, const fmatrix& pSignal, const cmatrix& sinusoids)
{
    for(int row = 0; row < pWeights.size(); row++)
    {
        fvec weights = dct(sinusoids, pSignal[row]);
        int maxInd = maxArg(weights, 0);
        for(int i = 0; i < maxInd; i++)
        {
            weights[i]= 0;
        }
        int offset = 0;
        
        if(maxInd > row)
        {
            offset = maxInd - row;
            for(int i = row; i < weights.size() - offset; i++)
            {
                pWeights[row][i] = weights[i + offset];
            }
        }
        else if(maxInd < row)
        {
            offset = row - maxInd;
            for(int i = row; i < weights.size(); i++)
            {
                pWeights[row][i] = weights[i - offset];
            }
        }
        sumNormalize(weights);

        
    }
}

void setBlockLimits(fvec& limits, const fvec& freqs, float srate, float blockSize, float maxSize)
{
    float blockPeriod = blockSize /srate;
    for(int i = 0; i < freqs.size(); i++)
    {
        //limits[i] = 1+ int( ceil( (1.0/freqs[i]) / blockPeriod) );
        //limits[i] = std::min(limits[i], maxSize);
        
        //limits[i] = int( ceil( .025 / blockPeriod) );
        
        limits[i] = int(srate/freqs[i]);
    }
}

// Process loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



fvec getPeaks(const fvec& signal)
{
    //naive (but effective) peak finding expecting a smooth signal
    //the amplitude function in this file generates such signals

    fvec peaks(signal.size(),0);
    for(int i = 1; i < signal.size()-1; i++)
    {
        peaks[i] = (signal[i -1] < signal[i]) && (signal[i] > signal[i+1]) ? 1 : 0;
    }
    
    //ignore first and last element
    return peaks;
}




// The rest~~~~~~~~~~~~~~~~~~~~~~~~~

bool propertiesChanged(const ProcessProperties& props, const PropertyHistory& past)
{
    bool output = false;

    if( props.transBlocks != past.transBlocks
    || props.noiseGate != past.noiseGate 
    || props.mode != past.mode) 
    {
        output = true;
    }

    return output; 
}

void saveProperties( ProcessProperties& props, PropertyHistory& past)
{
    past.transBlocks = props.transBlocks;
    past.blockSize = props.blockSize;
    past.mode = props.mode;
    past.noiseGate = props.noiseGate;
    past.srate = props.srate;
}

