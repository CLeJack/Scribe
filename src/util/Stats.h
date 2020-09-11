#pragma once
#include "Head.h"


inline float mean(const fvec& arr)
{
    float output = 0;
    for(int i= 0; i < arr.size(); i++)
    {
        output += arr[i];
    }

    return output/arr.size();
}

inline float rms(const fvec& arr, int start, int end)
{
    double total = 0;
    double val = 0;
    for(int i = start; i < end; i++)
    {
        total += std::pow(arr[i], 2);
    }

    return std::sqrt(total/arr.size());
}

inline float rms(const fvec& arr, int start)
{
    return rms(arr, start, arr.size());
}

inline float rms(const fvec& arr)
{
    return rms(arr, 0, arr.size());
}


inline float maxValue(const fvec& arr, int start, int end)
{
    float max = arr[start];

    for(int i = start; i < end; i++)
    {
        max = arr[i] > max ? arr[i] : max;
    }

    return max;
}

inline float maxValue(const fvec& arr, int start)
{
    return maxValue(arr, start, (int)arr.size());
}

inline float maxValue(const fvec& arr)
{
    return maxValue(arr, 0, (int)arr.size());
}


inline float absMaxValue(const fvec& arr, int start, int end)
{
    float max = std::abs(arr[start]);
    float other = 0;
    for(int i = start; i < end; i++)
    {
        other = std::abs(arr[i]); 
        max = other > max ? other : max;
    }

    return max;
}

inline float absMaxValue(const fvec& arr, int start)
{
    return absMaxValue(arr, start, (int)arr.size());
}

inline float absMaxValue(const fvec& arr)
{
    return absMaxValue(arr, 0, (int)arr.size());
}


inline int maxArg(const fvec& arr, int start, int end)
{
    int max = start;

    for(int i = start; i < end; i++)
    {
        max = arr[i] > arr[max] ? i : max;
    }
    return max;
}

inline int maxArg(const fvec& arr, int start)
{
    return maxArg(arr, start, (int)arr.size());
}

inline int maxArg(const fvec& arr)
{
    return maxArg(arr, 0, (int)arr.size());
}


inline float minValue(const fvec& arr, int start, int end)
{
    float min = arr[start];

    for(int i = start; i < end; i++)
    {
        min = arr[i] < min ? arr[i] : min;
    }

    return min;


}

inline float minValue(const fvec& arr, int start)
{

    return minValue(arr, start, arr.size());
}

inline float minValue(const fvec& arr)
{

    return minValue(arr, 0, arr.size());
}



inline int minArg(const fvec& arr, int start, int end)
{
    int min = start;

    for(int i = start; i < end; i++)
    {
        min = arr[i] < arr[min] ? i : min;
    }
    return min;
}

inline int minArg(const fvec& arr, int start)
{
    return minArg(arr, start, arr.size());
}

inline int minArg(const fvec& arr)
{
    return minArg(arr, 0, arr.size());
}



inline fvec absMaxNormalize(const fvec& arr)
{
    float max = absMaxValue(arr, 0);
    fvec output(arr.size(), 0);
    if(max > 0)
    {
        for(int i =0; i< arr.size(); i++)
        {
            output[i] = arr[i]/max;
        }
    }
    return output;
}

inline fvec sumNormalize(const fvec& arr)
{
    float total = 0;
    fvec output(arr.size(), 0);
    
    for(int i = 0; i<arr.size(); i++)
    {
        total += arr[i];
    }

    if(total > 0)
    {
        for(int i = 0; i<arr.size(); i++)
        {
            output[i] = arr[i]/total;
        }

    }
    return output;
}

inline float sum(const fvec& arr)
{
    float total =0;

    for(int i = 0; i < arr.size(); i++)
    {
        total += arr[i];
    }

    return total;
}

inline fvec pctDiff(const fvec& arr)
{
    fvec output(arr.size(), 0);

    for(int i = 1; i < output.size(); i++)
    {
        if(arr[i-1] == 0)
        {
            output[i] = 1;
        }
        else
        {
            output[i] = (arr[i] - arr[i-1])/arr[i-1];
        }
    }
    return output;
}

//simple linear regression
inline fvec linreg(const fvec& x, const fvec& y)
{
    float numerator = 0;
    float denominator = 0;
    float xmean = mean(x);
    float ymean = mean(y);
    float xdev = 0;

    fvec output(2,0);

    for(int i = 0; i < x.size(); i++)
    {
        xdev = (x[i] - xmean) ;
        numerator += xdev * (y[i] - ymean);
        denominator += xdev * xdev;
        
    }
    output[0] = numerator/denominator; //slope
    output[1] = ymean - output[0] * xmean; //intercept
    return output;

}

// in my case none of the statistical properties of x changes
// all are precalculated
inline fvec linreg(const fvec& xerr, float xvar, float xmean, const fvec& y)
{
    float numerator = 0;
    float ymean = mean(y);
    fvec output(2,0);

    for(int i = 0; i < xerr.size(); i++)
    {
        numerator += xerr[i] * (y[i] - ymean);
    }
    output[0] = numerator/xvar; 
    output[1] = ymean - output[0] * xmean;
    return output;
}


inline fvec buildLinRegX(int size, int center)
{
    float normCenter = (float)center/size;
    fvec output(size, 0);
    for(int i = 0; i < size; i++)
    {
        output[i] = (float)i/size - normCenter;
    }   
    return output;
}

inline fvec line(const fvec& regOutput, int len)
{
    fvec output(len, 0);

    for(int i = 0; i < len; i++)
    {
        output[i] = regOutput[0] *i + regOutput[1];
    }

    return output;
}

//center of mass

inline float CoMY(const fvec& arr)
{
    float numerator = 0;
    float denominator = 0;
    for(int i = 0; i < arr.size(); i++)
    {
        numerator += std::abs(arr[i]) * i;
        denominator += i;
    }
    return numerator/denominator;
}

inline fvec CoMY2(const fvec& arr)
{
    float numerator1 = 0;
    float denominator1 = 0;
    float numerator2 = 0;
    float denominator2 = 0;
    int half = arr.size()/2;

    fvec output(2,0);

    for(int i = 0; i < half; i++)
    {
        numerator1 += std::abs(arr[i] * i);
        denominator1 += i;
    }

    for(int i = half; i < arr.size(); i++)
    {
        numerator2 += std::abs(arr[i] * i);
        denominator2 += i;
    }

    output[0] = (numerator1 + numerator2)/(denominator1 + denominator2);
    output[1] = numerator2/denominator2;
    return output;
}

inline float CoMX(const fvec& arr)
{
    float numerator = 0;
    float denominator = 0;
    for(int i = 0; i < arr.size(); i++)
    {
        numerator += std::abs(arr[i] * i);
        denominator += std::abs(arr[i]);
    }
    return numerator/denominator;
}

inline fvec CoMX2(const fvec& arr)
{
    float numerator1 = 0;
    float denominator1 = 0;
    float numerator2 = 0;
    float denominator2 = 0;
    int half = arr.size()/2;

    fvec output(2,0);

    for(int i = 0; i < half; i++)
    {
        numerator1 += std::abs(arr[i] * i);
        denominator1 += std::abs(arr[i]);
    }

    for(int i = half; i < arr.size(); i++)
    {
        numerator2 += std::abs(arr[i] * i);
        denominator2 += std::abs(arr[i]);
    }

    output[0] = (numerator1 + numerator2)/(denominator1 + denominator2);
    output[1] = numerator2/denominator2;
    return output;
}

inline float getPeriod(float freq, float srate, int size)
{
    return int(size - srate/freq)+1;
}

inline fvec diff(const fvec& arr)
{
    fvec output(arr.size(), 0);

    for(int i = 1; i < arr.size(); i++)
    {
        output[i] = arr[i] - arr[i-1];
    }

    return output;
}