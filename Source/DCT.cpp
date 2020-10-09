#include "DCT.h"

// Discrete Customized Transform

void dct(fvec& weights, const cmatrix& matrix, const fvec& signal, 
int rowi, int rowf, int indi, int indf)
{
    rowi = std::max(rowi,0);
    rowf = std::min(rowf, (int)matrix.size());
    for(int row = rowi; row < rowf; row++)
    {
        std::complex<float> total(0.0f,0.0f);
        
        for(int sample = indi; sample < indf; sample++)
        {   
            total += signal[sample] * matrix[row][sample];
        }
        
        weights[row] = std::abs(total);
    }
}


fvec dct(const cmatrix& matrix, const fvec& signal, 
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

fvec dct(const cmatrix& matrix, const fvec& signal)
{   // can pass exMatrix with exWeights

    int rowf = matrix.size();
    int indf = signal.size();

    return dct( matrix, signal, 0, rowf, 0, indf);
}

fvec dctSignalAvg(const fvec& weights, float signalSize)
{
    fvec output(weights.size(), 0);
    for(int i = 0; i < output.size(); i++)
    {
        output[i]/signalSize;
    }

    return output;
}


void setComplexMatrix(cmatrix& matrix, const fvec& freqs, const fvec& timeVector)
{
    std::complex<float> iunit(0,1);
    for(int  row = 0; row < freqs.size(); row++)
    {
        float omega = 2 * MY_PI * freqs[row];
        

        for(int col = 0; col < timeVector.size(); col++)
        {
            matrix[row][col] = std::exp(iunit * (omega * timeVector[col]));
        }
    }
}