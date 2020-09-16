#include "CircularBuffer.h"

int main()
{
    fvec in1 = {1,2,3,4,5};

    fvec in2 = {5,4,3,2,1};

    FloatBuffer buff(5,0);

    fvec output = buff.toOrderedVec();

    for(int i= 0; i < output.size(); i++)
    {
        std::cout <<output[i]<<", ";
    }

    std::cout<<"\n";




    for(int i = 0; i < in1.size(); i++)
    {
        buff.push(in1[i]);
    }

    output = buff.toOrderedVec();

    for(int i= 0; i < output.size(); i++)
    {
        std::cout <<output[i]<<", ";
    }
    std::cout<<"\n";




    for(int i = 0; i < 3; i++)
    {
        buff.push(in2[i]);
    }


    output = buff.toOrderedVec();

    for(int i= 0; i < output.size(); i++)
    {
        std::cout <<output[i]<<", ";
    }

    std::cout<<"\n";

    return 0;

    
}