#pragma once

#include "Head.h"

struct FloatBuffer
{
    FloatBuffer(int size, float fill);
    const int size;
    void fill(float value, float percentage);
    fvec toOrderedVec();

    void push(float val);

    float lastValue();
    float currentValue();

    float mean(int lastXelements);
    
    fvec vec;
    int head = 0;
};