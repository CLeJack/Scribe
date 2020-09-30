#include "ProcessData.h"

// struct setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`

void Scribe::initialize(float srate, float blockSize) 
{
    namespace SA = Scribe::Audio;
    namespace SD = Scribe::DownSample;

    SA::historySamples = int(SA::historyTime * SA::srate) + 1;
    SA::srate          = srate;
    SA::blockSize      = blockSize;

    Scribe::isInitialized = true;

    SD::factor    = SA::srate / SD::srate;
    SD::blockSize = SA::blockSize / SD::factor;

    setTimeVector(Scribe::timeVector, SD::srate);
    setComplexMatrix(Scribe::matrix, Scribe::frequencies, Scribe::timeVector);
}


void updateRangeCalcs()
{
    namespace CR = Calculations::Range;

    CR::lowNote  = AudioParams::lowNote;
    CR::highNote = CR::lowNote + 48; //4 octaves
}


//pass in order vec from the float buffer
void updateSignalCalcs(const fvec& signal)
{
    namespace C = Calculations;
    namespace S = Scribe;
    namespace A = AudioParams;

    //center of mass w.r.t. signal y axis. Very similar to RMS, but with faster response;
    fvec comy3 = CoMY3(signal);

    C::Amp::amp = comy3[0];
    C::Amp::half1 = comy3[1]; // oldest half of the signal
    C::Amp::half2 = comy3[2]; // newest half

    //fundamental index chosen by the greatest peak in frequency spectrum
    C::Fundamental::index = maxArg (S::weights);
    C::Fundamental::ratio = S::ratios [C::Fundamental::index];

    C::weight = S::weights [C::Fundamental::index];

    C::retrigger = C::Amp::half2 / C::Amp::amp; //compare the full signal amplitude with the earliest half

    
    C::Amp::dB = int16ToDb(C::Amp::amp);

    C::Delay::amp = SMA (C::Delay::amp, C::Amp::amp,
        secToBlocks (A::SmoothTime::amp, S::Audio::srate, S::Audio::blockSize));
    
    C::Delay::amp = std::max(C::Delay::amp, -60.0f); //infinity introduces errors in SMA


    C::Amp::dB = SMA (C::Delay::dB, C::Amp::dB,
        secToBlocks (A::SmoothTime::dB, S::Audio::srate, S::Audio::blockSize));

    
    C::Angle::amp = std::atan (C::Amp::amp / C::Delay::amp) * 180 / MY_PI;
    C::Angle::dB = std::atan  (C::Amp::dB  / C::Delay::dB ) * 180 / MY_PI;


    // note Ind undergoes an octave correction if necessary;
    // weight data can be distributed between 1st, 2nd and 3rd harmonics in some instances
    // this is the only way I can see to fix this until I implement statistical methods
    C::Note::index = C::Fundamental::index;
    C::Note::ratio = C::Fundamental::ratio;
    if ( C::Fundamental::ratio < A::Threshold::ratio)
    {
        C::Note::index -= S::Tuning::octaveSize;
        C::Note::ratio = S::ratios [C::Note::index];
    }

    //This must be calculated after the note has been properly updated
    C::Threshold::weight = weightLimit(
        A::Threshold::weight,
        A::Scale::weight,
        A::lowNote,
        C::Note::index,
        S::Tuning::octaveSize);

    //This must be calculated after the note has been properly updated
    C::Threshold::noise = noiseLimit(
        A::Threshold::noise,
        A::Scale::noise,
        A::lowNote,
        C::Note::index,
        S::Tuning::octaveSize);


    C::Fundamental::octave /= S::Tuning::octaveSize;
    C::Fundamental::pitch  %= S::Tuning::octaveSize;

    C::Note::octave /= S::Tuning::octaveSize;
    C::Note::pitch %= S::Tuning::octaveSize;
}

void updateMidiNum() 
{
    namespace C = Calculations;
    namespace S = Scribe;
    namespace A = AudioParams;

    int ind = C::Note::index;
    ind = C::weight    < C::Threshold::weight ? 0 : ind;
    ind = C::Delay::dB < C::Threshold::noise  ? 0 : ind;
    ind = ind < 0 ? 0 : ind;

    C::Midi::index = midiShift(ind, S::frequencies, S::Tuning::refFreq, A::Shift::octave, A::Shift::semitone);
    C::Midi::velocity = midiVelocity(A::Velocity::max, A::Velocity::min, C::Angle::amp, A::Angle::amp); 
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