#include "CircularBuffer.h"


FloatBuffer::FloatBuffer(int size, float fill) : vec(size,fill), size(size)
{
}

//int FloatBuffer::size(){ return vec.get()->size(); }

void FloatBuffer::fill(float value, float percentage)
{
    //int len = vec.get()->size();
    int len = size;
    int start = 0;
    if(percentage < 1)
    {
        start = len - (len * percentage);
    }
    
    for(int i = 0; i < start; i++)
    {
        vec[i] = 0;
    }
    for(int i = start; i < len; i++)
    {
        vec[i] = value;
    }

    head = start;
}

fvec FloatBuffer::toOrderedVec()
{
    fvec output(size,0);

    for(int i = 0; i < size ; i++)
    {
        output[i] = vec[(i + head) % size];        
    }

    return output;
}

void FloatBuffer::push(float val)
{
    vec[head] = val;
    head = (head + 1) % size;
}


float FloatBuffer::mean(int lastXelements)
{
    //obviously bad if input < 1 or more than vec size
    fvec window = toOrderedVec();
    float total = 0;
    for(int i = window.size() - lastXelements; i < window.size(); i++)
    {
        total += window[i];
    }

    return total/lastXelements;
}


float FloatBuffer::lastValue()
{
    if(head == 0){ return vec[ size-1]; }

    return vec[head -1];
}

float FloatBuffer::currentValue()
{
    return vec[head];
}