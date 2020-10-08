#pragma once
#include "ModelInstances.h"
#include "Debug.h"

#define PRINT 1


int main()
{

    float srate = 44100;
    float blockSize = 128;

    scribe.initialize(srate, blockSize);

    fvec signal0 = importCsv("input/_test_a3.csv", 2.5*srate);

    #if PRINT == 1
    printRows(scribe.frequencies, "output/_0_freqs.csv");
    printColumn(scribe.timeVector, "output/_0_timeVector.csv");

    printMatrixReal(scribe.matrix, "output/_0_cmatrix.csv",0);

    printColumn(signal0, "output/_1_signal.csv");

    #endif

    int loops = (signal0.size() / scribe.audio.blockSize);
    int start = 0;
    int end = 0;

    params.threshold.noise0 = -68;
    params.threshold.noise1 = -68;
    params.threshold.noise2 = -68;
    params.threshold.noise3 = -68;

    for(int i = 0; i < loops; i++)
    {
        fvec block(scribe.audio.blockSize, 0);
        int offset = i * scribe.audio.blockSize;
        for(int i = 0; i < scribe.audio.blockSize; i++)
        {
            //continually pushing to the down sample history gives a really bad signal
            //even with filtering, so use the full history and down sample from there always
            scribe.history.get()->push( signal0[i + offset] );
        }
        
        fvec trueSignal = scribe.history.get()->toOrderedVec();

        for(int i = 0; i < scribe.historyDS.size(); i++)
        {
            scribe.historyDS[i] = trueSignal[i * scribe.audio.ds.factor];
        }

        
        calcs.updateRange(params.range);

        dct(scribe.weights, scribe.matrix, scribe.historyDS,
            calcs.range.lowNote, calcs.range.highNote, 
            scribe.audio.ds.signalStart, scribe.historyDS.size());

        sumNormalize(scribe.weights);
        weightRatio(scribe.ratios, scribe.weights, scribe.tuning.octaveSize);
        
        calcs.updateSignal (scribe, params);
        calcs.updateMidi   (scribe, params);

        SwitchMessage message{};
        
        MidiParams midiParams = getMidiParams(calcs);

        message = scribe.midiSwitch.update(midiParams);
        

        
#if PRINT == 1
        fvec output = {
            (float)calcs.fundamental.index,
            (float)calcs.note.index,
            (float)calcs.targets.weight,
            calcs.threshold.weight,
            (float)calcs.fundamental.ratio,
            (float)calcs.note.ratio,
            calcs.amp.val,
            calcs.amp.dB,
            calcs.delay.dBShort,
            calcs.delay.dBLong,
            calcs.threshold.noise,
            calcs.targets.retrigger,
            (float)message.on,
            (float)message.onVel,
            (float)message.off,
            (float)message.offVel,
            (float)message.send,
            scribe.midiSwitch.notes.current,
            scribe.midiSwitch.notes.prev,
            (float)scribe.midiSwitch.state};
            

        //printRows( trueSignal, "_2_history.csv");
        printRows( scribe.historyDS, "output/_2_historyDS.csv");
        printRows( scribe.weights, "output/_2_weights.csv");
        printRows( scribe.ratios, "output/_2_ratios.csv");
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
