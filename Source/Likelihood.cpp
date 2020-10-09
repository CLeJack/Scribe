
#include "Likelihood.h"

void setSineMatrix(fmatrix& sineMat, const cmatrix& matrix, const fvec& timeVector, 
    const fvec& freqs, void (*normFunc) (fvec&))
{
    fvec signal;

    for(int row = 0; row < sineMat.size(); row++)
    {
        signal = sinusoid(timeVector, freqs[row], 0, 1);

        sineMat[row] = dct(matrix, signal);

        normFunc(sineMat[row]);
    }
}

void setOctErrMatrix1(fmatrix& errMat, const cmatrix& matrix, const fvec& timeVector, 
    const fvec& freqs, void (*normFunc) (fvec&), int octSize)
{
    fvec signal1;
    fvec signal2;
    fvec sigSum;

    float amp1 = 0.5f;
    float amp2 = 1.0f;

    for(int row = 0; row < errMat.size(); row++)
    {
        signal1 = sinusoid(timeVector, freqs[row], 0, amp1);
        
        if(row + octSize < freqs.size())
        {
            signal2 = sinusoid(timeVector,freqs[row+octSize],0, amp2);
            sigSum = add(signal1, signal2);
        }
        else
        {
            sigSum = signal1;
        }

        errMat[row] = dct(matrix, sigSum);

        normFunc(errMat[row]);
    }
}


fvec weightErr(fvec& weights, const fmatrix& matrix, 
int rowi, int rowf, int indi, int indf, float (*statFunc) (const fvec&))
{
    fvec output(weights.size(), 0);
    
    rowi = std::max(rowi,0);
    rowf = std::min(rowf, (int)matrix.size());
    for(int row = rowi; row < rowf; row++)
    {   
        fvec errs(weights.size(), 0);
        for(int sample = indi; sample < indf; sample++)
        {   
            errs[sample] = std::abs(weights[sample] - matrix[row][sample]);            
        }

        output[row] = statFunc(errs);
    }

    return output;
}

fvec pctErr(fvec& weights, const fmatrix& matrix, 
int rowi, int rowf, int indi, int indf, float (*statFunc) (const fvec&))
{
    fvec output(weights.size(), 0);
    
    rowi = std::max(rowi,0);
    rowf = std::min(rowf, (int)matrix.size());
    for(int row = rowi; row < rowf; row++)
    {   
        fvec errs(weights.size(), 0);
        for(int sample = indi; sample < indf; sample++)
        {   
            errs[sample] = std::abs(weights[sample] - matrix[row][sample]) / matrix[row][sample];            
        }

        output[row] = statFunc(errs);
    }

    return output;
}

fvec normalizeErr(fvec & errs, const fmatrix& matrix)
{
    fvec output(errs.size(), 0);

    for(int i = 0; i < output.size(); i++)
    {
        output[i] = errs[i] / sum(matrix[i]);
    }

    return output;
}