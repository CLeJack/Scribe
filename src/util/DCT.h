#pragma once
#include "Head.h"

// Discrete Customized Transform

inline fvec dct(const cmatrix& matrix, const fvec& signal, 
                int rowi, int rowf, int indi, int indf)
{
    rowi = std::max(rowi,0);
    rowf = std::min(rowf, (int)matrix.size());
    fvec weights(matrix.size(),0);
    for(int row = rowi; row < rowf; row++)
    {
        std::complex<float> total(0.0f,0.0f);
        
        for(int sample = indi; sample < indf; sample++)
        {   
            total += signal[sample] * matrix[row][sample];
        }
        
        weights[row] = std::abs(total);
    }
    return weights;
}

inline fvec dct(const cmatrix& matrix, const fvec& signal)
{   // can pass exMatrix with exWeights

    int rowf = matrix.size() - 1;
    int indf = signal.size();

    return dct( matrix, signal, 0, rowf, 0, indf);
}

inline fvec dctSignalAvg(const fvec& weights, float signalSize)
{
    fvec output(weights.size(), 0);
    for(int i = 0; i < output.size(); i++)
    {
        output[i]/signalSize;
    }

    return output;
}
/*
def dct_concentration(dctamp, octsize = 12):
    maxind = 0
    total = 0
    output =  np.zeros(dctamp.shape)
    truemax = np.argmax(dctamp) - 2 * octsize

    for i in range(1, len(dctamp)-1):
        if (dctamp[i-1] < dctamp[i] and dctamp[i] > dctamp[i+1]) :
            maxind = i

        if dctamp[i-1] > dctamp[i] and dctamp[i] < dctamp[i+1] and i >= truemax:
            output[maxind] = total
            total = 0
            maxind = 0
        else:
            total += dctamp[i]

    return output
*/
inline fvec dctConcentration(const fvec& dctamp)
{
    int maxInd = 0;
    float total = 0;

    fvec output(dctamp.size(),0);

    for(int i = 1; i < dctamp.size()-1; i++)
    {
        if(dctamp[i-1] < dctamp[i] && dctamp[i] > dctamp[i+1])
        {
            maxInd = i;
        }

        if(dctamp[i-1] > dctamp[i] && dctamp[i] < dctamp[i+1])
        {
            output[maxInd] = total;
            total = 0;
            maxInd = 0;
        }
        else
        {
            total += dctamp[i];
        }
    }

    return output;

}


inline void setComplexMatrix(cmatrix& matrix, const fvec& freqs, const fvec& timeVector)
{
    std::complex<float> iunit(0,1);
    for(int  row = 0; row < freqs.size(); row++)
    {
        float omega = 2 * M_PI * freqs.at(row);
        

        for(int col = 0; col < timeVector.size(); col++)
        {
            matrix[row][col] = std::exp(iunit * (omega * timeVector[col]));
        }
    }
}

inline void setCSquareMatrix(cmatrix& matrix, const fvec& freqs, const fvec& timeVector)
{
    //This helps clean up misreads from chords
    //and somewhat reduces harmonic errors
    std::complex<float> iunit(0,1);
    for(int  row = 0; row < freqs.size(); row++)
    {
    
        float omega = 2 * M_PI * freqs.at(row);
        

        for(int col = 0; col < timeVector.size(); col++)
        {
            matrix[row][col] = std::exp(iunit * (omega * timeVector[col]));
            matrix[row][col].real( matrix[row][col].real() > 0 ? 1.0f : -1.0f);
            matrix[row][col].imag( matrix[row][col].imag() > 0 ? 1.0f : -1.0f);
        }
    }
}