#include "ProcessData.h"

// struct setup ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`

namespace Scribe {

	void initialize(float srate, float blockSize)
    {
        namespace SA = Scribe::Audio;
        namespace SD = Scribe::DownSample;
        namespace ST = Scribe::Tuning;

        
        SA::srate          = srate;
        SA::blockSize      = blockSize;
        SA::historySamples = int(SA::historyTime * SA::srate) + 1;
        

        SD::factor    = SA::srate / SD::srate;
        SD::blockSize = SA::blockSize / SD::factor;

        setETFrequencies(Scribe::frequencies, ST::refFreq, ST::semitone, ST::lowExp, ST::highExp);
        setTimeVector(Scribe::timeVector, SD::srate);
        setComplexMatrix(Scribe::matrix, Scribe::frequencies, Scribe::timeVector);

        history.reset(new FloatBuffer(SA::historySamples, 0.0f));

        Scribe::isInitialized = true;
    }
	bool isInitialized = false; //don't run PluginProcessor ready state loop without this set to true

	namespace Tuning 
	{
		const float refFreq = 440; //concert tuning
		const float semitone = float(std::pow(2, 1.0 / 12.0)); //12 Tone equal temperament
		const int octaveSize = 12;

		const int lowExp = -57; // -57 = C0
		const int highExp = 50 - 24; // 50 = B8; aliasing exist in octaves 7 to 8;
	}

	namespace Audio 
	{
		const float historyTime = .05f;
		int historySamples = 0;

		float srate = 44100;
		float blockSize = 128;
	}

	namespace DownSample 
	{
		const float srate = 4000;
		const int historySamples = int(Audio::historyTime * srate);

		//for use with DCT
		//full signal start calculates octave 0 which isn't needed
	    //half signal starts calculations on octave 1 which should be bass guitar range
		const int signalStart = historySamples / 2;

		int factor = 0;
		int blockSize = 0;
	}


	//arrays & buffers
	fvec frequencies = fvec (1 + Tuning::highExp - Tuning::lowExp, 0);
	fvec weights     = fvec (frequencies.size(), 0);
	fvec ratios      = fvec (frequencies.size(), 0);

	fvec timeVector  = fvec (DownSample::historySamples, 0);
	cmatrix matrix   = cmatrix (frequencies.size(), cvec (DownSample::historySamples, std::complex<float>(0, 0)));
	
	std::unique_ptr<FloatBuffer> history;
	fvec historyDS = fvec (DownSample::historySamples, 0.0001f);

    MidiSwitch midiSwitch = MidiSwitch();
}


void updateRangeCalcs()
{
    namespace CR = Calculations::Range;

    CR::lowNote  = AudioParams::Range::lowNote;
    CR::highNote = CR::lowNote + 48; //4 octaves
}


//pass in order vec from the float buffer
void updateSignalCalcs()
{
    namespace C = Calculations;
    namespace S = Scribe;
    namespace A = AudioParams;

    //center of mass w.r.t. signal y axis. Very similar to RMS, but with faster response;
    fvec comy3 = CoMY3(S::historyDS);

    C::Amp::amp = comy3[0];
    C::Amp::half1 = comy3[1]; // oldest half of the signal
    C::Amp::half2 = comy3[2]; // newest half

    //fundamental index chosen by the greatest peak in frequency spectrum
    C::Fundamental::index = maxArg (S::weights);
    C::Fundamental::ratio = S::ratios [C::Fundamental::index];

    C::weight = S::weights [C::Fundamental::index];

    C::retrigger = C::Amp::half2 / C::Amp::amp; //compare the full signal amplitude with the earliest half

    
    C::Amp::dB = int16ToDb(C::Amp::amp);
    C::Amp::dB = C::Amp::dB < -90 ? -90 : C::Amp::dB;

    C::Blocks::amp = secToBlocks (A::SmoothTime::amp, S::Audio::srate, S::Audio::blockSize);
    C::Blocks::dB = secToBlocks (A::SmoothTime::dB, S::Audio::srate, S::Audio::blockSize);
    C::Blocks::midi = secToBlocks (A::SmoothTime::midi, S::Audio::srate, S::Audio::blockSize);

    C::Delay::amp = SMA (C::Delay::amp, C::Amp::amp, C::Blocks::amp);

    C::Delay::dB = SMA (C::Delay::dB, C::Amp::dB, C::Blocks::dB);


    C::Angle::amp = std::atan (C::Amp::amp / C::Delay::amp) * 180 / MY_PI;
    C::Angle::dB = std::atan  (C::Amp::dB  / C::Delay::dB ) * 180 / MY_PI;


    // note Ind undergoes an octave correction if necessary;
    // weight data can be distributed between 1st, 2nd and 3rd harmonics in some instances
    // this is the only way I can see to fix this until I implement statistical methods
    C::Note::index = C::Fundamental::index;
    C::Note::ratio = C::Fundamental::ratio;
    if ( C::Fundamental::ratio < A::Threshold::ratio 
      && C::Fundamental::index >= S::Tuning::octaveSize)
    {
        C::Note::index -= S::Tuning::octaveSize;
        C::Note::ratio = S::ratios [C::Note::index];
    }

    //This must be calculated after the note has been properly updated
    C::Threshold::weight = weightLimit(
        A::Threshold::weight,
        A::Scale::weight,
        A::Range::lowNote,
        C::Note::index,
        S::Tuning::octaveSize);

    //This must be calculated after the note has been properly updated
    C::Threshold::noise = noiseLimit(
        A::Threshold::noise,
        A::Scale::noise,
        A::Range::lowNote,
        C::Note::index,
        S::Tuning::octaveSize);


    C::Fundamental::octave /= S::Tuning::octaveSize;
    C::Fundamental::pitch  %= S::Tuning::octaveSize;

    C::Note::octave /= S::Tuning::octaveSize;
    C::Note::pitch %= S::Tuning::octaveSize;
}

void updateMidiCalcs() 
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

void weightRatio(fvec& ratios, const fvec& arr, int octSize)
{

    for(int i = octSize; i < arr.size(); i++)
    {
        ratios[i] = arr[i] /arr[i-octSize];
        ratios[i] = ratios[i] > 10 ? 10 : ratios[i];
    }
}



MidiParams getMidiParams()
{
    namespace C = Calculations;
    namespace A = AudioParams;

    auto output = MidiParams();

    output.weightVal = C::weight;
    output.weightThresh = C::Threshold::weight;

    output.midiNum = C::Midi::index;
    output.delaydB = C::Delay::dB;
    output.noiseThresh = C::Threshold::noise;
    output.releaseThresh = C::Threshold::release;

    output.retrigVal = C::retrigger;
    output.retrigStart = A::Threshold::retrigStart;
    output.retrigStop = A::Threshold::retrigStop;
    
    output.velocityVal = C::Midi::velocity;
    
    output.smoothFactor = C::Blocks::midi;

    return output;
}