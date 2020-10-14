#include "Debug.h"

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

void printRows(const std::vector<bool>& arr, std::string title)
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
//