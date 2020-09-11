#pragma once
#include "ProcessData.h"

#define PRINT 0


int main()
{
    
    auto props = ProcessProperties(44100, 128);

    auto data = ProcessData(props);
    
    PropertyHistory propHist;
    saveProperties (props, propHist);
    
    fvec signal0 = importCsv("_test_a3.csv", 2.5*44100);

    #if PRINT == 1
    printRows(*data.frequencies.get(), "_0_freqs.csv");
    printRows(*data.exFrequencies.get(), "_0_exfreqs.csv");
    printRows(*data.midiNumbers.get(), "_0_midi.csv");
    printColumn(*data.timeVector.get(), "_0_timeVector.csv");

    printMatrixReal(*data.matrix.get(), "_0_cmatrix.csv",0);
    printMatrix(*data.exMatrix.get(), "_0_excmatrix.csv",0);
    
    printMatrix(*data.kernels.get(), "_0_kernels.csv",0);
    printMatrix(*data.kernelWeights.get(), "_0_kernelWeights.csv",0);
    printRows(*data.noteLimits.get(), "_0_limits.csv");

    printColumn(signal0, "_1_signal.csv");

    #endif

    int loops = (signal0.size() / props.blockSize);
    int start = 0;
    int end = 0;

    float noiseP = -40;
    float releaseP = -60;
    float weightP = .05;
    float octaveThresh = 2;
    float retrigStartP = .9;
    float retrigStopP = .99;
    int smoothP = 4;
    int octShift = 0;
    int semitoneShift = 0;
    int lowestNoteP = 28;
    int highestNoteP = 72;

    float velDbMinP = -40;
    float velDbMaxP = -20;
    int velMinP = 32;
    int velMaxP = 96;

    MidiSwitch midiSwitch = MidiSwitch();


    for(int i = 0; i < loops; i++)
    {
        fvec block(props.blockSize, 0);
        int offset = i*props.blockSize;
        for(int i = 0; i < props.blockSize; i++)
        {
            //continually pushing to the down sample history gives a really bad signal
            //even with filtering, so use the full history and down sample from there always
            data.history.get()->push( signal0[i + offset] );
        }
        
        fvec trueSignal = data.history.get()->toOrderedVec();
        
        fvec signalDS(props.dsHistSamples,0);

        for(int i = 0; i < signalDS.size(); i++)
        {
            signalDS[i] = trueSignal[i*props.dsFactor];
        }

        
        int windowStart = signalDS.size() - signalDS.size()/2;
        fvec weights = dct(*data.matrix.get(), signalDS, lowestNoteP, highestNoteP, 
                           0, signalDS.size());

        weights = sumNormalize(weights);
        fvec ratios = weightRatio(weights, 12);
        int f0ind = maxArg(weights);

        if(ratios[f0ind] < octaveThresh)
        {
            f0ind = f0ind-12 < 0 ? 0 : f0ind - 12;
        }

        fvec comy2 = CoMY2(signalDS);
        float avgPeakAmp = 0.5*(maxValue(signalDS) - minValue(signalDS));

        

        float metricTrigger= weights[f0ind];
        float metricRetrigger = comy2[1]/comy2[0];
        float amp = int16ToDb(avgPeakAmp);
        

        f0ind = metricTrigger < weightP ? 0 : f0ind;
        f0ind = f0ind < 24 ? 0 : f0ind;
        f0ind = amp < noiseP ? 0 : f0ind;
        

        float midiNum = 0;
        MidiSwitch& midiSwitch = *data.midiSwitch.get();
        SwitchMessage message{};
        
        midiNum = midiShift(f0ind, *data.midiNumbers.get(), octShift, semitoneShift);
        MidiParams p{midiNum, amp, noiseP, releaseP, 
        metricRetrigger, retrigStartP, retrigStopP, 
        velDbMinP, velDbMaxP, velMinP, velMaxP,
        smoothP};
        message = midiSwitch.update(p);
        

        
#if PRINT == 1
        fvec output = {
            (float)f0ind,
            comy2[0],
            avgPeakAmp,
            int16ToDb(comy2[0]),
            int16ToDb(avgPeakAmp),
            metricRetrigger,
            metricTrigger,
            ratios[f0ind],
            (float)message.on,
            (float)message.onVel,
            (float)message.off,
            (float)message.offVel,
            (float) message.send,
            midiSwitch.notes.current,
            midiSwitch.notes.prev,
            (float)midiSwitch.state};
            

        //printRows( trueSignal, "_2_history.csv");
        printRows( signalDS, "_2_historyDS.csv");
        printRows( weights, "_2_weights.csv");
        printRows( ratios, "_2_ratios.csv");
        printRows(output, "_2_value_output.csv");

#elif PRINT == 2

    std::cout<<(float)f0ind <<", "<<
            midiNum <<", "<<
            comy <<", "<<
            ampDiff <<", "<<
            metric <<", "<<
            (float)message.on <<", "<<
            (float)message.onVel <<", "<<
            (float)message.off <<", "<<
            (float)message.offVel <<", "<<
            (float) message.send <<", "<<
            midiSwitch.notes.current <<", "<<
            midiSwitch.notes.prev <<", "<<
            (float)midiSwitch.state <<"\n";
#endif

    }
    return 0;
}

#undef PRINT
