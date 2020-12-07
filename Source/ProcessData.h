#pragma once
#include "Head.h"

inline float secToFraction(float seconds, float srate, float blockSize, float timeDivisor = 1)
{
	// second to blocks
	// set time divisor to 1000 for milliseconds, or 1 for seconds 
	// srate (samples/sec) * seconds / blockSize (samples/block) + 1;
	// gives a minimum of 1 block when seconds = 0
	return srate * (seconds/timeDivisor) / blockSize;
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

