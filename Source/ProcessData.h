#pragma once

#include "Head.h"
#include "Waveforms.h"
#include "Tuning.h"
#include "CircularBuffer.h"
#include "Stats.h"
#include "DCT.h"
#include "MidiSwitch.h"
#include "AudioParams.h"
#include "Calculations.h"





namespace Scribe {

	void initialize(float srate, float blockSize);//set all required non-const variables here
	bool isInitialized = false; //don't run PluginProcessor ready state loop without this set to true

	namespace Tuning 
	{
		const float refFreq = 440; //concert tuning
		const float semitone = float(std::pow(2, 1.0 / 12.0)); //12 Tone equal temperament
		const int octaveSize = 12;

		const int lowExp = -57; // -57 = C0
		const int highExp = 50 - 24 + 1; // 50 = B8; aliasing exist in octaves 7 to 8;
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
		const int historySamples = int(historyTime * dsRate);

		//for use with DCT
		//full signal start calculates octave 0 which isn't needed
	    //half signal starts calculations on octave 1 which should be bass guitar range
		const int signalStart = historySamples / 2;

		int factor = 0;
		int blockSize = 0;
	}


	//arrays & buffers
	fvec frequencies = fvec (highExp - lowExp, 0);
	fvec weights     = fvec (frequencies.size(), 0);
	fvec ratios      = fvec (frequencies.size(), 0);

	fvec timeVector  = fvec (Scribe::DownSample::historySamples, 0);
	cmatrix matrix   = cmatrix (frequencies.size(), cvec (Scribe::DownSample::historySamples, std::complex<float>(0, 0)));
	
	std::unique_ptr<FloatBuffer> history;
	fvec historyDS = fvec (Scribe::DownSample::historySamples, 0.0001f);

	//midi state machine
	MidiSwitch midiSwitch;
}

void updateRangeCalcs();
void updateAudioParams();

//pass in order vec from the float buffer
void updateSignalCalcs(const fvec& signal);
void updateMidiCalcs();
MidiParams getMidiParams();


inline int secToBlocks(float seconds, float srate, float blockSize, float timeDivisor = 1000.0f)
{
	// second to blocks
	// set time divisor to 1000 for milliseconds, or 1 for seconds 
	// srate (samples/sec) * seconds / blockSize (samples) + 1;
	// gives a minimum of 1 block when seconds = 0
	return srate * (seconds/timeDivisor) / blockSize + 1;
}

inline float weightLimit(float minWeight, float weightScale, float minIndex, float currentIndex, float octSize) 
{
	// log plot of weight limits that increases by a factor of weightScale 
	// for each octave above the lowest allowed index
	// and offset by minWeight
	return minWeight + weightScale * std::log10 ( (currentIndex - minIndex) / octSize);
}

inline float noiseLimit(float minNoise, float noiseScale, float minIndex, float currentIndex, float octSize) 
{
	return noiseScale * ((currentIndex - minIndex) / octSize) + minNoise;
}

fvec getPeaks(const fvec& signal);
void alignWithPeaks(fvec& arr, const fvec& peaks);

void clearBelowInd(fvec& arr, int ind);
void clearAboveInd(fvec& arr, int ind);

fvec weightRatio(const fvec& arr, int octSize);


