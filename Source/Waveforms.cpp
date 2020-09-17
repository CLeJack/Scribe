#include "Waveforms.h"


fvec getTimeVector(float time, float srate)
{
    int size = int(0.5 + srate/time) + 1;
    std::vector<float> output(size,0);

    float interval = time/srate;
    for(int i = 1; i < size; i++)
    {
        output[i] = output[i-1] + interval;
    }

    return output;
}

fvec getTimeVector(int samples, float srate)
{
    //notice the daw Reaper was initializing these with 
    //samples = 1, and srate = 0; not sure why though.
    std::vector<float> output(samples, 0);

    float interval = 1/srate;
    for(int i = 1; i < samples; i++)
    {
        output[i] = output[i-1] + interval;
    }

    //timeVector(float, int) causes segfaults due to +1 rounding
    //do not pass time and srate to it to create outputs for this function
    return output;
}

void setTimeVector(fvec& timeVector, float srate)
{
    for(int i = 0; i < timeVector.size(); i++)
    {
        // 1 / srate = time increment
        timeVector[i] = (i * (1.0f / srate) );
    }
}

fvec sinusoid(const fvec& t, float frequency, float phaseDeg, float amplitude)
{
    //t = time vector
    fvec output(t.size(),0);

    float omega = 2 * MY_PI * frequency;
    float theta = phaseDeg * MY_PI/180;

    for(int i = 0; i < output.size(); i++)
    {
        output[i] = amplitude * sin(omega * t[i] + theta);
    }

    return output;
}

fvec sinusoid(float time, float srate, float frequency, float phaseDeg, float amplitude)
{
    fvec t = getTimeVector(time, srate);
    return sinusoid(t, frequency, phaseDeg, amplitude);
}

fvec sinusoid(int samples, float srate, float frequency, float phaseDeg, float amplitude)
{
    fvec t = getTimeVector(samples ,srate);
    return sinusoid(t, frequency, phaseDeg, amplitude);
}

cvec complex_sinusoid(const fvec& t, float frequency, float phaseDeg, float amplitude)
{
    cvec output(t.size(), std::complex<float>(0,0));

    float omega = 2 * MY_PI * frequency;
    float theta = phaseDeg * MY_PI/180;
    std::complex<float> iunit(0,1);

    for(int i = 0; i < output.size(); i++)
    {
        output[i] = amplitude * std::exp(iunit * (omega * t[i] + theta));
    }
    
    return output;
}


cvec complex_sinusoid(float time, float srate, float frequency, float phaseDeg, float amplitude)
{
    fvec t = getTimeVector(time, srate);
    return complex_sinusoid(t, frequency, phaseDeg, amplitude);
}

cvec complex_sinusoid(int samples, float srate, float frequency, float phaseDeg, float amplitude)
{
    fvec t = getTimeVector(samples ,srate);
    return complex_sinusoid(t, frequency, phaseDeg, amplitude);
}


float gaussian(float x, float amplitude, float fwhm)
{
    //fwhm = full width at half maximum
    return amplitude * std::exp( -std::pow( x/fwhm, 2 ));   
}


fvec gaussianComb(const fvec& t, float freq, float phaseDeg, float amplitude, float fwhm)
{
    fvec xs = sinusoid(t, freq, phaseDeg, amplitude);

    fvec output(xs.size(), 0);

    for(int i = 0; i < xs.size(); i++)
    {
        output[i] = gaussian(xs[i], amplitude, fwhm);
    }

    return output;

}

fmatrix gaussianOctaves(const fvec& frequencies, int octSize)
{
    fmatrix output(frequencies.size(), fvec(frequencies.size(),0));
    float x = 0;
    float amp = 1;
    int half = octSize/2;
    int quarter = octSize/4;
    int third = octSize/3;

    for(int row = 0; row < output.size(); row++)
    {
        
        for(int i = 0; i < frequencies.size(); i++)
        {
            x = ((i - row + quarter) % half) - quarter;
            x = x * third;
            output[row][i] = gaussian(x,amp,third);
        }
    }
    return output;
}

