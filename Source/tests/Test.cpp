#pragma once
#include "ProcessData.h"
#include "Debug.h"

#define PRINT 1


int main()
{
    
    auto props = Properties(44100, 128);

    auto store = Storage(props);
    
    fvec signal0 = importCsv("input/_test_amfull.csv", 2.5*44100);

    #if PRINT == 1
    printRows(*store.frequencies.get(), "output/_0_freqs.csv");
    printRows(*store.exFrequencies.get(), "output/_0_exfreqs.csv");
    printColumn(*store.timeVector.get(), "output/_0_timeVector.csv");

    printMatrixReal(*store.matrix.get(), "output/_0_cmatrix.csv",0);
    printMatrix(*store.exMatrix.get(), "output/_0_excmatrix.csv",0);

    printColumn(signal0, "output/_1_signal.csv");

    #endif

    int loops = (signal0.size() / props.blockSize);
    int start = 0;
    int end = 0;

    AudioParams audioParams;

    audioParams.noise = -30;
    audioParams.release = -60;
    audioParams.weight = .05f;
    audioParams.octStr = 3;
    audioParams.retrigStart = .9f;
    audioParams.retrigStop = 1.0f;
    audioParams.smooth = 4;
    audioParams.octave = 0;
    audioParams.semitone = 0;
    audioParams.loOct = 2;
    audioParams.velDbMax = -20;
    audioParams.velDbMin = -30;
    audioParams.velMax = 127;
    audioParams.velMin = 0;

    MidiSwitch midiSwitch = MidiSwitch();


    for(int i = 0; i < loops; i++)
    {
        fvec block(props.blockSize, 0);
        int offset = i*props.blockSize;
        for(int i = 0; i < props.blockSize; i++)
        {
            //continually pushing to the down sample history gives a really bad signal
            //even with filtering, so use the full history and down sample from there always
            store.history.get()->push( signal0[i + offset] );
        }
        
        fvec trueSignal = store.history.get()->toOrderedVec();
        
        fvec signalDS(props.dsHistSamples,0);

        for(int i = 0; i < signalDS.size(); i++)
        {
            signalDS[i] = trueSignal[i*props.dsFactor];
        }

        Calculations calcs;

        calcs.updateRangeInfo(audioParams, signalDS.size());

        int windowStart = signalDS.size() - signalDS.size()/2;
        fvec weights = dct(*store.matrix.get(), signalDS,
            calcs.loNote, calcs.hiNote, calcs.signalStart, signalDS.size());

        weights = sumNormalize(weights);
        fvec ratios = weightRatio(weights, 12);
        
        calcs.updateSignalInfo(weights, ratios, signalDS, audioParams);
        calcs.updateMidiNum(store, props, audioParams);

        MidiSwitch& midiSwitch = *store.midiSwitch.get();
        SwitchMessage message{};
        
        MidiParams midiParams = getMidiParams(calcs, audioParams);

        message = midiSwitch.update(midiParams);
        

        
#if PRINT == 1
        fvec output = {
            (float)calcs.f0ind,
            calcs.ampFull,
            calcs.ampHalf,
            calcs.ampdB,
            calcs.trigger,
            calcs.retrigger,
            calcs.f0ratio,
            calcs.noteRatio,
            (float)message.on,
            (float)message.onVel,
            (float)message.off,
            (float)message.offVel,
            (float) message.send,
            midiSwitch.notes.current,
            midiSwitch.notes.prev,
            (float)midiSwitch.state};
            

        //printRows( trueSignal, "_2_history.csv");
        printRows( signalDS, "output/_2_historyDS.csv");
        printRows( weights, "output/_2_weights.csv");
        printRows( ratios, "output/_2_ratios.csv");
        printRows(output, "output/_2_value_output.csv");

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
