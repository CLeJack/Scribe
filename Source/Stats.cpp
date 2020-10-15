#include "Stats.h"

//Basic Stats~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

float mean(const fvec& arr, int start, int end)
{
    float output = 0;
    for(int i= start; i < end; i++)
    {
        output += arr[i];
    }

    return output/(float)arr.size();
}

float mean(const fvec& arr, int start)
{
    return mean(arr, start, arr.size());
}

float mean(const fvec& arr)
{
    return mean(arr, 0, arr.size());
}


float positiveMean(const fvec& arr, int start, int end)
{
    float output = 0;
    float count = 1;
    for(int i= start; i < end; i++)
    {
        output += arr[i];
        count += 1;
    }

    return output/count;
}
//~~Root mean Squared ~~~~~~~~~~~~~~~~~~~~~~~
float rms(const fvec& arr, int start, int end)
{
    double total = 0;
    double val = 0;
    for(int i = start; i < end; i++)
    {
        total += std::pow(arr[i], 2);
    }

    return std::sqrt(total/arr.size());
}

float rms(const fvec& arr, int start)
{
    return rms(arr, start, arr.size());
}

float rms(const fvec& arr)
{
    return rms(arr, 0, arr.size());
}

//~~Max~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float maxValue(const fvec& arr, int start, int end)
{
    float max = arr[start];

    for(int i = start; i < end; i++)
    {
        max = arr[i] > max ? arr[i] : max;
    }

    return max;
}

float maxValue(const fvec& arr, int start)
{
    return maxValue(arr, start, (int)arr.size());
}

float maxValue(const fvec& arr)
{
    return maxValue(arr, 0, (int)arr.size());
}


float absMaxValue(const fvec& arr, int start, int end)
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

float absMaxValue(const fvec& arr, int start)
{
    return absMaxValue(arr, start, (int)arr.size());
}

float absMaxValue(const fvec& arr)
{
    return absMaxValue(arr, 0, (int)arr.size());
}


int maxArg(const fvec& arr, int start, int end)
{
    int max = start;

    for(int i = start; i < end; i++)
    {
        max = arr[i] > arr[max] ? i : max;
    }
    return max;
}

int maxArg(const fvec& arr, int start)
{
    return maxArg(arr, start, (int)arr.size());
}

int maxArg(const fvec& arr)
{
    return maxArg(arr, 0, (int)arr.size());
}

//~~Min~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
float minValue(const fvec& arr, int start, int end)
{
    float min = arr[start];

    for(int i = start; i < end; i++)
    {
        min = arr[i] < min ? arr[i] : min;
    }

    return min;


}

float minValue(const fvec& arr, int start)
{

    return minValue(arr, start, arr.size());
}

float minValue(const fvec& arr)
{

    return minValue(arr, 0, arr.size());
}



int minArg(const fvec& arr, int start, int end)
{
    int min = start;

    for(int i = start; i < end; i++)
    {
        min = arr[i] < arr[min] ? i : min;
    }
    return min;
}

int minArg(const fvec& arr, int start)
{
    return minArg(arr, start, arr.size());
}

int minArg(const fvec& arr)
{
    return minArg(arr, 0, arr.size());
}


//~~Normalization~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fvec absMaxNormalize(const fvec& arr, int zero)
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

void absMaxNormalize(fvec& arr)
{
    float max = absMaxValue(arr, 0);
    if(max > 0)
    {
        for(int i =0; i< arr.size(); i++)
        {
            arr[i] = arr[i]/max;
        }
    }
}

fvec sumNormalize(const fvec& arr, int zero)
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


void sumNormalize(fvec& arr)
{
    float total = 0;
    
    for(int i = 0; i<arr.size(); i++)
    {
        total += arr[i];
    }

    if(total > 0)
    {
        for(int i = 0; i<arr.size(); i++)
        {
            arr[i] = arr[i]/total;
        }

    }
}

//~~misc. basic ops~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float sum(const fvec& arr)
{
    float total =0;

    for(int i = 0; i < arr.size(); i++)
    {
        total += arr[i];
    }

    return total;
}

fvec pctDiff(const fvec& arr)
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

fvec diff(const fvec& arr)
{
    fvec output(arr.size(), 0);

    for(int i = 1; i < arr.size(); i++)
    {
        output[i] = arr[i] - arr[i-1];
    }

    return output;
}

//simple linear regression ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fvec linreg(const fvec& x, const fvec& y)
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

fvec line(const fvec& regOutput, int len)
{
    fvec output(len, 0);

    for(int i = 0; i < len; i++)
    {
        output[i] = regOutput[0] *i + regOutput[1];
    }

    return output;
}

//center of mass ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

float CoMY(const fvec& arr)
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

fvec CoMY2(const fvec& arr)
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

fvec CoMY3(const fvec& arr)
{
    float numerator1 = 0;
    float denominator1 = 0;
    float numerator2 = 0;
    float denominator2 = 0;
    int half = arr.size()/2;

    fvec output(3,0);

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
    output[1] = numerator1/denominator1; //oldest history w.r.t. time
    output[2] = numerator2/denominator2; //newest history w.r.t. time
    return output;
}

float CoMX(const fvec& arr)
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

fvec CoMX2(const fvec& arr)
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

float getPeriod(float freq, float srate, int size)
{
    return int(size - srate/freq)+1;
}


float SMA(float hist, float val, float size)
{
    /*
    it seems this method has two vunerabilities

    1. it holds on to data introduced into the history much longer than if an actual window was used
       e.g. if 1 million was introduced with a prev_avg of 1, many updates would be needed to flush 1 million.
       If a window of size N with history was used, only N+1 updates would be needed.

    2. rounding error is introduced with each division.

    log(0) introduces infinities, but rounding error shouldn't be an issue.
    */
   float frac = 1.0f/size;
    return hist * (1 - frac) + (val * frac);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

fvec add(const fvec& arr1, const fvec& arr2)
{
    fvec output(arr1.size(), 0);

    for(int i = 0; i < arr1.size(); i++)
    {
        output[i] = arr1[i] + arr2[i];
    }

    return output;
}

fvec hadamardX(const fvec& arr1, const fvec& arr2)
{
    fvec output(arr1.size(), 0);

    for(int i = 0; i < arr1.size(); i++)
    {
        output[i] = arr1[i] * arr2[i];
    }

    return output;
}