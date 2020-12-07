#pragma once

#include "Head.h"

template <class T>
class Buffer
{
public:
    const int size;
    std::vector<T> vec;
    int head = 0;

    Buffer(int size, T fill) : size(size), vec(size, fill)
    {
    };


    std::vector<T> toOrderedVec()
    {
        std::vector<T> output(size, 0);

        for (int i = 0; i < size; i++)
        {
            output[i] = vec[(i + head) % size];
        }

        return output;
    };

    void push(T val)
    {
        vec[head] = val;
        head = head + 1 < size ? head + 1 : 0;
    };

    T lastValue()
    {
        if (head == 0) { return vec[size - 1]; }

        return vec[head - 1];
    };

    T currentValue()
    {
        return vec[head];
    }

    T& operator[](int i)
    {
        int ind = head + i >= size ? head + i - size : head + i;
        return vec[ind];
    }


};