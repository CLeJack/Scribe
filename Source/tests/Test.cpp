#pragma once
#include "ProcessData.h"
#include "Debug.h"

#define PRINT 1


int main()
{
    
    namespace S = Scribe;
    namespace A = AudioParams;
    namespace C = Calculations;

    float srate = 44100;
    float blockSize = 128;

    S::initialize(srate, blockSize);

    fvec signal0 = importCsv("input/_test_ab4seq.csv", 2.5*srate);

    #if PRINT == 1
    printRows(S::frequencies, "output/_0_freqs.csv");
    printColumn(S::timeVector, "output/_0_timeVector.csv");

    printMatrixReal(S::matrix, "output/_0_cmatrix.csv",0);

    printColumn(signal0, "output/_1_signal.csv");

    #endif

    int loops = (signal0.size() / S::Audio::blockSize);
    int start = 0;
    int end = 0;

    A::Threshold::noise = -35;
    A::Threshold::release = -50;
    A::Threshold::weight = .1f;
    A::Threshold::ratio = 3;
    A::Threshold::retrigStart = 0.9f;
    A::Threshold::retrigStop = 1.0f;
    A::SmoothTime::midi = 11;
    A::SmoothTime::amp = 11;
    A::SmoothTime::dB = 11;
    A::Shift::octave = 0;
    A::Shift::semitone = 0;
    
    A::Angle::amp = 55;
    A::Velocity::max = 127;
    A::Velocity::min = 0;
    A::Range::lowNote = 28;

    

    for(int i = 0; i < loops; i++)
    {
        fvec block(S::Audio::blockSize, 0);
        int offset = i * S::Audio::blockSize;
        for(int i = 0; i < S::Audio::blockSize; i++)
        {
            //continually pushing to the down sample history gives a really bad signal
            //even with filtering, so use the full history and down sample from there always
            S::history.get()->push( signal0[i + offset] );
        }
        
        fvec trueSignal = S::history.get()->toOrderedVec();

        for(int i = 0; i < S::historyDS.size(); i++)
        {
            S::historyDS[i] = trueSignal[i * S::DownSample::factor];
        }

        
        updateRangeCalcs();

        dct(S::weights, S::matrix, S::historyDS,
            C::Range::lowNote, C::Range::highNote, 
            S::DownSample::signalStart, S::historyDS.size());

        sumNormalize(S::weights);
        weightRatio(S::ratios, S::weights, 12);
        
        updateSignalCalcs();
        updateMidiCalcs();

        SwitchMessage message{};
        
        MidiParams midiParams = getMidiParams();

        message = S::midiSwitch.update(midiParams);
        

        
#if PRINT == 1
        fvec output = {
            (float)C::Fundamental::index,
            (float)C::Note::index,
            (float)C::weight,
            C::Threshold::weight,
            (float)C::Fundamental::ratio,
            (float)C::Note::ratio,
            C::Amp::amp,
            C::Delay::amp,
            C::Amp::dB,
            C::Delay::dB,
            C::Threshold::noise,
            C::Angle::amp,
            C::retrigger,
            (float)message.on,
            (float)message.onVel,
            (float)message.off,
            (float)message.offVel,
            (float)message.send,
            S::midiSwitch.notes.current,
            S::midiSwitch.notes.prev,
            (float)S::midiSwitch.state};
            

        //printRows( trueSignal, "_2_history.csv");
        printRows( S::historyDS, "output/_2_historyDS.csv");
        printRows( S::weights, "output/_2_weights.csv");
        printRows( S::ratios, "output/_2_ratios.csv");
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
