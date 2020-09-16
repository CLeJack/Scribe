#include "CircularBuffer.h"


FloatBuffer::FloatBuffer(int size, float fill)
{
    this->size = size;
    vec.reset(new fvec(std::max(size,1),fill));
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
        vec.get()->at(i) = 0;
    }
    for(int i = start; i < len; i++)
    {
        vec.get()->at(i) = value;
    }

    head = start;
}

fvec FloatBuffer::toOrderedVec()
{
    fvec output(size,0);

    for(int i = 0; i < size ; i++)
    {
        output[i] = vec.get()->at( (i + head) % size);        
    }

    return output;
}

void FloatBuffer::push(float val)
{
    vec.get()->at(head) = val;
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
    if(head == 0){ return vec.get()->at( size-1); }

    return vec.get()->at(head -1);
}

float FloatBuffer::currentValue()
{
    return vec.get()->at(head);
}