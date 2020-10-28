#pragma once
#include "ModelInstances.h"
#include "Debug.h"

#define PRINT 1


int main()
{

    float srate = 44100;
    float blockSize = 128;

    scribe.initialize(srate, blockSize);

    fvec signal0 = importCsv("input/trem.csv", 2.5*srate);

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

        scribe.updateFundamental(calcs.range, calcs.blocks, 
                             calcs.amp, calcs.threshold);

        //scribe.updateFMidiInfo(calcs.threshold, calcs.amp, calcs.velocity, 
        //                      calcs.range, calcs.shift, calcs.blocks);

        /*
        scribe.updateChords(calcs.range, calcs.blocks, 
                             calcs.amp, calcs.threshold);

        scribe.updateCMidiInfo(calcs.threshold, calcs.amp, calcs.velocity, 
                              calcs.range, calcs.shift);
        */

       SwitchMessage message{};
       
       MidiParams midiParams = getMidiParams(calcs, scribe);
       
       message = scribe.midiSwitch.update(midiParams);
        

        
#if PRINT == 1
        fvec output = {
            (float)scribe.fundamental.index,
            (float)scribe.fundamental.prevIndex,
            calcs.delay.dBShort,
            calcs.delay.dBLong,
            calcs.threshold.noise,
            999,
            calcs.amp.dB,
            calcs.threshold.release,
            999,
            (float)calcs.range.lowNote,
            (float)calcs.range.highNote,
            999,
            calcs.amp.retrig,
            calcs.threshold.retrig,
            calcs.threshold.retrigStop,
            999,
            (float)scribe.midiSwitch.state,
            (float)scribe.midiSwitch.notes.current,
            (float)scribe.midiSwitch.notes.prev,
            (float)message.on,
            (float)message.off,
            999,
            float(scribe.fundamental.prevIndex != scribe.fundamental.index),
            calcs.threshold.chordPct,
            scribe.peakFloor,
            999,
            (float)calcs.amp.slope,
            (float)scribe.fundamentalCertainty[scribe.fundamental.index],
            (float)scribe.fundamentalHistory[scribe.fundamental.index]
            };
            

        //printRows( trueSignal, "_2_history.csv");
        //printRows( scribe.historyDS, "output/_2_historyDS.csv");
        //printRows( scribe.weights, "output/_2_weights.csv");
        //printRows( scribe.maxWeights, "output/_2_maxNormW.csv");
        //printRows( scribe.fundamentalHistory, "output/_2_maxWHist.csv");
        //printRows( scribe.chordHistory, "output/_2_chordHist.csv");
        //printRows( scribe.peaksHistory, "output/_2_peakWHist.csv");
        //printRows( scribe.peaks, "output/_2_peaks.csv");
        //printRows( scribe.fOnNotes, "output/_2_fOnNotes.csv");
        //printRows( scribe.cOnNotes, "output/_2_cOnNotes.csv");
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
/*
    for(int i = 0; i < scribe.fNeedsRelease.size(); i++)
        {
            if(scribe.fNeedsTrigger[i])
            {
                scribe.turnOnMidi(i, calcs.amp, calcs.threshold);
            }
            if(scribe.fNeedsRelease[i])
            {
                scribe.turnOffMidi(i);
            }
        }
        */
    }
    return 0;
}

#undef PRINT
