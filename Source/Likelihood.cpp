
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


fmatrix freqCertaintyMatrix(const fvec& maxWeights, const fmatrix& maxMatrix, 
int rowi, int rowf, int indi, int indf)
{
    fmatrix output(maxMatrix.size(), fvec(maxWeights.size(), 0));

    rowi = std::max(rowi,0);
    rowf = std::min(rowf, (int)maxMatrix.size());
    for(int row = rowi; row < rowf; row++)
    {   
        
        for(int sample = indi; sample < indf; sample++)
        {   
            /*
            maxWeights is bound on [0, 1] and represents the real freq spectrum
            maxMatrix is bound on [0, 1] and represents an ideal freq spectrum
            for each frequency represented by a row in the matrix.

            subtracting naxWeights from a maxMatrix gives a % error since both are already
            mapped to the probability space.

            Consequently: 1 - err gives a weight which can be used in a later function.

            Consider the following.

            Given: a row and sample from the maxMatrix (which is ideal) that reads 100%
            while the same sample in the maxWeights vector reads 10%.

            The error between them is 90%. 100% - 90% gives me a 10% weight with the reasoning
            being if the real value is 90% off from the ideal, I don't want to count it as much
            in the final evaluation.

            */
            output[row][sample] = 1 - std::abs(maxWeights[sample] - maxMatrix[row][sample]);            
        }

    }

    return output;
}


fvec freqCertaintyVector(const fvec& sumWeights, const fmatrix& freqMatrix,
int rowi, int rowf, int indi, int indf)
{
    fvec output(sumWeights.size(), 0);

    rowi = std::max(rowi,0);
    rowf = std::min(rowf, (int)freqMatrix.size());
    for(int row = rowi; row < rowf; row++)
    {   
        
        for(int sample = indi; sample < indf; sample++)
        {   
            /*
            sumWeights is bound on [0,1], it sums to one, and it is a snapshot
            of the real freqSpectrum of the current signal that is being analyzed.

            freqMatrix is bounds on [0,1] and each row
            represents a series of weights associated with each frequency of interest.

            This function is just a dot product as a result.

            If the row in freqMatrix has a high certainty, then every weight
            will be near 1, and the product of sumWeights will be 1.

            the sum of weighted values divided by the sum of original values 
            will give a final certainty, since I know the original value sums to 1, 
            and the weighted values are between 0 and 1.

            */
            output[row] += sumWeights[sample] * freqMatrix[row][sample];            
        }

    }

    return output;
}