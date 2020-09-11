#pragma once

#include "Head.h"

struct FloatBuffer
{
    FloatBuffer(int size, float fill);
    int size;
    void fill(float value, float percentage);
    fvec toOrderedVec();

    void push(float val);

    float lastValue();
    float currentValue();

    float mean(int lastXelements);
    
    std::unique_ptr<fvec> vec;
    int head = 0;
};