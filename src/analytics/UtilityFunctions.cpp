#pragma once
#include "UtilityFunctions.h"
#include "Waveforms.h"


//debugging
void printColumn(fvec& arr, std::string title)
{
    std::ofstream file;
    file.open(title, std::fstream::in | std::fstream::out | std::fstream::app);
    for(int i = 0; i < arr.size(); i++)
    {
        file << arr[i]<<"\n";
    }
    file.close();
}



void printColumn(float val, std::string title)
{
    std::ofstream file;
    file.open(title, std::fstream::in | std::fstream::out | std::fstream::app);
    file << val<<"\n";
    file.close();
}

void printRows(const fvec& arr, std::string title)
{
    std::ofstream file;
    file.open(title, std::fstream::in | std::fstream::out | std::fstream::app);
    for(int i = 0; i < arr.size(); i++)
    {
        file << arr[i]<<",";
    }
    file<<"\n";
    file.close();
}

void printRows(const ivec& arr, std::string title)
{
    std::ofstream file;
    file.open(title, std::fstream::in | std::fstream::out | std::fstream::app);
    for(int i = 0; i < arr.size(); i++)
    {
        file << arr[i]<<",";
    }
    file<<"\n";
    file.close();
}

void printMatrix(fmatrix& mat, std::string title, int id)
{
    std::ofstream file;
    file.open(title, std::fstream::in | std::fstream::out | std::fstream::app);
    file <<id<<"\n";
    for(int row = 0; row < mat.size(); row++)
    {
        for(int col = 0; col < mat[row].size(); col++)
        {
            file << mat[row][col]<<",";
        }
        file<<"\n";
        
    }
    file.close();
    
}

void printMatrix(cmatrix& mat, std::string title, int id)
{
    std::ofstream file;
    file.open(title, std::fstream::in | std::fstream::out | std::fstream::app);
    file <<id<<"\n";
    for(int row = 0; row < mat.size(); row++)
    {
        for(int col = 0; col < mat[row].size(); col++)
        {
            file << mat[row][col]<<"|";
        }
        file<<"\n";
        
    }
    file.close();
    
}

void printMatrixReal(cmatrix& mat, std::string title, int id)
{
    std::ofstream file;
    file.open(title, std::fstream::in | std::fstream::out | std::fstream::app);
    file <<id<<"\n";
    for(int row = 0; row < mat.size(); row++)
    {
        for(int col = 0; col < mat[row].size(); col++)
        {
            file << mat[row][col].real()<<",";
        }
        file<<"\n";
        
    }
    file.close();
    
}

fvec importCsv(std::string filename, int vecsize)
{
    auto output = fvec(vecsize,0);

    std::string line;

    std::ifstream file;
    file.open(filename);

    int i = 0;
    while(getline(file,line))
    {
        if(i < vecsize)
        {
            output[i] = std::stof(line);
        }
        else
        {
            break;
        }
        i++;
    }
    file.close();
    return output;
}

void printLine(fvec& arr, std::string title)
{
    std::cout << title << "\n";
    for(int i = 0; i < arr.size(); i++)
    {
        std::cout<< arr[i] << ", ";
    }

    std::cout <<"\n";
}

void printLine(ivec& arr, std::string title)
{
    std::cout << title << "\n";
    for(int i = 0; i < arr.size(); i++)
    {
        std::cout<< arr[i] << ", ";
    }

    std::cout <<"\n";
}

void printElement(float val)
{
    std::cout << val << ", ";
}
//Non debugging~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~``

fvec crush(const fvec& exArr, int finalSize, int shift, int factor)
{   //default shift = 1, factor = 3
    fvec output(finalSize,0);
    int ind = 0;
    for(int exInd = 0; exInd < exArr.size(); exInd++)
    {
        ind =int((exInd + shift)/factor);

        //choose the best value between all potential values at this index
        output[ind] = std::max(output[ind],exArr[exInd]);
    }

    return output;
}

void clearBelowInd(fvec& arr, int ind)
{
    for(int i = 0; i < ind; i++)
    {
        arr[i] = 0;
    }
}

void clearAboveInd(fvec& arr, int ind)
{
    for(int i = ind + 1; i < arr.size(); i++)
    {
        arr[i] = 0;
    }
}

void clearQuietNotes(fvec& amps, float noiseThresh)
{
    for(int i = 0; i < amps.size(); i++)
    {
        amps[i] = amps[i] < noiseThresh? 0 : amps[i];
    }
}

void alignWithPeaks(fvec& arr, const fvec& peaks)
{
    for(int i = 0; i < arr.size(); i++)
    {
        arr[i] = arr[i] * peaks[i];
    }
}



fvec getAmplitudes(const fvec& weights, float amp)
{
    fvec output(weights.size(),0);
    for(int i = 0; i < weights.size(); i++)
    {
        output[i] = weights[i] * amp;
    }
    return output;
}

fvec getPitchClassWeights(const fvec& weights, int octSize, int start)
{
    start = std::min(start, (int)weights.size() - octSize);//don't go out of bounds
    fvec output(octSize,0);
    for(int i = start; i < (int)weights.size(); i++)
    {
        output[i % octSize] += weights[i];
    }

    return output;
}

fvec getOctaveWeights(const fvec& weights, int octSize)
{
    fvec output(int(weights.size()/octSize),0);
    for(int i = 0; i < weights.size(); i++)
    {
        output[int(i/octSize)] += weights[i];
    }

    return output;
}

void joinIntervals(fvec& arr, int semitones)
{
    for(int i = 0; i < arr.size() - semitones; i++)
    {
        arr[i] = arr[i] + arr[i+semitones];
    }
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

void updateVec(fvec& target, const fvec& newValues)
{
    for(int i = 0; i < newValues.size(); i++)
    {
        target[i] = newValues[i];
    }
}

void fill(fvec& arr, float value)
{
    for(int i = 0; i < arr.size(); i++)
    {
        arr[i] = value;
    }
}




float normToInt16Range(float val)
{
    return val*SINT16;
}

float int16ToDb(float amp)
{
    return 20 * log10( amp/SINT16);
}

float dbToInt16(float db)
{
    return std::pow(10, db/20.0f)* SINT16;
}


fvec weightRatio(const fvec& arr, int octSize)
{
    fvec output(arr.size(), 0);

    for(int i = octSize; i < arr.size(); i++)
    {
        output[i] = arr[i] /arr[i-octSize];
    }

    return output;
}