#pragma once

#include "Head.h"
#include "Waveforms.h"
#include "Tuning.h"
#include "CircularBuffer.h"
#include "Stats.h"
#include "DCT.h"
#include "MidiSwitch.h"
#include "Globals.h"



//Scribe program
namespace Scribe {

	void initialize(float srate, float blockSize);//set all required non-const variables here
	extern bool isInitialized; //don't run PluginProcessor ready state loop without this set to true

	namespace Tuning 
	{
		extern const float refFreq; //concert tuning
		extern const float semitone; //12 Tone equal temperament
		extern const int octaveSize;

		extern const int lowExp; // -57 = C0
		extern const int highExp; // 50 = B8; aliasing exist in octaves 7 to 8;
	}

	namespace Audio 
	{
		extern const float historyTime;
		extern int historySamples;

		extern float srate;
		extern float blockSize;
	}

	namespace DownSample 
	{
		extern const float srate;
		extern const int historySamples;

		//for use with DCT
		//full signal start calculates octave 0 which isn't needed
	    //half signal starts calculations on octave 1 which should be bass guitar range
		extern const int signalStart;

		extern int factor;
		extern int blockSize;
	}


	//arrays & buffers
	extern fvec frequencies;
	extern fvec weights;
	extern fvec ratios;

	extern fvec timeVector;
	extern cmatrix matrix;
	
	extern std::unique_ptr<FloatBuffer> history;
	extern fvec historyDS;

	//midi state machine
	extern MidiSwitch midiSwitch;
}



void updateRangeCalcs();
void updateAudioParams();

//pass in order vec from the float buffer
void updateSignalCalcs();
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
	float x = (currentIndex - minIndex) /octSize;
	x = x < 0 ? 0 : x;
	return minWeight + weightScale * std::log10 ( 1 + x) / 100;
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

void weightRatio(fvec& ratios, const fvec& arr, int octSize);

