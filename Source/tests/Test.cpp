#pragma once
#include "ModelInstances.h"
#include "Debug.h"

#define PRINT 1


int main()
{

    float srate = 44100;
    float blockSize = 128;

    scribe.initialize(srate, blockSize);

    fvec signal0 = importCsv("input/_test_ab4seq.csv", 2.5*srate);

    #if PRINT == 1
    printRows(scribe.frequencies, "output/_0_freqs.csv");
    printColumn(scribe.timeVector, "output/_0_timeVector.csv");

    printMatrixReal(scribe.matrix, "output/_0_cmatrix.csv",0);
    printMatrix(scribe.maxSineMatrix, "output/_0_maxsinematrix.csv",0);
    printMatrix(scribe.maxOctMatrix, "output/_0_maxOctMatrix.csv",0);

    //printColumn(signal0, "output/_1_signal.csv");

    #endif

    int loops = (signal0.size() / scribe.audio.blockSize);
    int start = 0;
    int end = 0;

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

        calcs.updateRange(scribe, params);

        calcs.updateSignal(scribe, params);

        scribe.updateWeights(calcs.range.lowNote, calcs.range.highNote, calcs.blocks.midi, 
                             calcs.amp, calcs.threshold);

        scribe.updateMidiInfo(calcs.threshold, calcs.amp, calcs.velocity, 
                              calcs.range, calcs.shift);

        calcs.updateFundamental(scribe);



        for(int i = 0; i < scribe.needsRelease.size(); i++)
        {
            if(scribe.needsTrigger[i])
            {
                scribe.turnOnMidi(i);
            }
            if(scribe.needsRelease[i])
            {
                scribe.turnOffMidi(i);
            }
        }
        

        
#if PRINT == 1
        fvec output = {
            (float)scribe.fundamental.index,
            (float)scribe.fundamental.history,
            calcs.delay.dBShort,
            calcs.delay.dBLong,
            calcs.amp.factor,
            calcs.threshold.noise,
            calcs.threshold.release,
            maxValue(scribe.certainty)};
            

        //printRows( trueSignal, "_2_history.csv");
        //printRows( scribe.historyDS, "output/_2_historyDS.csv");
        //printRows( scribe.weights, "output/_2_weights.csv");
        //printRows( scribe.maxWeights, "output/_2_maxNormW.csv");
        printRows( scribe.weightHistory, "output/_2_maxWHist.csv");
        printRows( scribe.chordHistory, "output/_2_chordWHist.csv");
        //printRows( scribe.onNotes, "output/_2_onNotes.csv");
        printRows(output, "output/_2_value_output.csv");
        //

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
