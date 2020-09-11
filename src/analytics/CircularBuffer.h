#pragma once
#include "Head.h"
#include "Stats.h"

struct FloatBuffer
{
    FloatBuffer(int size, float fill);
    int size;
    void fill(float value, float percentage);
    fvec toOrderedVec();
    fvec toSquareWave();
    fvec toPulseVec(float threshold);

    void push(float val);

    float lastValue();
    float currentValue();

    float mean(int lastXelements);
    float noZerosMean(int lastXElements);
    //float median(int lastXelements);
    
    std::unique_ptr<fvec> vec;
    int head = 0;
};