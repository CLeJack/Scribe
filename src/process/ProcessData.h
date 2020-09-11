#include "Head.h"
#include "Waveforms.h"
#include "Tuning.h"
#include "UtilityFunctions.h"
#include "CircularBuffer.h"
#include "Kernels.h"
#include "Stats.h"
#include "Transients.h"
#include "MidiSwitch.h"
#include "DCT.h"

struct PropertyHistory
{
    float transBlocks = 1;
    float blockSize = 128;
    float srate = 44100;
    float noiseGate = 0;
    Mode mode = mono;

};

struct ProcessProperties : public PropertyHistory
{
    ProcessProperties();
    ProcessProperties(float srate, float blockSize);
    const float refFreq = 440; //concert tuning
    const float semitone = std::pow(2, 1.0/12.0); //12 Tone equal temperament
    const int octaveSize = 12;
    const std::vector<std::string> noteNames =       {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"};
    const std::vector<std::string> sharpNotesNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    const std::vector<std::string> flatNoteNames =   {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
    const float histTime = .05;
    

    //float lowExp = -57 + 12; // -57 = C0
    int lowExp = -57; // -57 = C0
    int highExp = 50 - 24 + 1; // 50 = B8; aliasing exist in octaves 7 to 8;

    //the frequencies bins I have setup are already aligned with midi;
    int midiOffset = lowExp + 69;
    int freqSize = 0;
    int histSamples = 0;
    int transBlocks = 1;

    

    float dsRate = 4000;
    int dsFactor = 0;
    int dsHistSamples = 0;
    int dsBlock = 0;

    //used in the main loop to allow for cleaner downsampling;
    int dsIndex = 0;
    
};



struct ProcessData
{
    ProcessData();
    ProcessData(ProcessProperties& props);
    std::unique_ptr<fvec> frequencies;
    std::unique_ptr<fvec> exFrequencies;
    std::unique_ptr<fvec> midiNumbers;
    std::unique_ptr<fmatrix> kernels;
    std::unique_ptr<fmatrix> kernelWeights;
    std::unique_ptr<fvec> noteLimits;

    std::unique_ptr<fvec> timeVector;

    std::unique_ptr<cmatrix> matrix;
    std::unique_ptr<cmatrix> exMatrix;

    std::unique_ptr<FloatBuffer> history;
    std::unique_ptr<FloatBuffer> historyDS;

    std::unique_ptr<MidiSwitch> midiSwitch;
    
};


//Process flow

// Start and block audio loop to set vectors
void setETFrequencies(fvec& freqs, float refFreq, float semitone, int lowExp, int highExp);

void expandFrequencies(fvec& freqs, fvec& exFreqs);

fvec getOctFrequencies(const fvec& refFreqs, int octSize);



void setSineWeights(fmatrix& weights, const cmatrix& matrix, const fvec& freqs, const fvec& timeVector, int clearLast);

void setTimeVector(fvec& timeVector, float dSrate);

void setParabolicKernels(fmatrix& kernels, const fvec& freqs, float dSrate);


void setBlockLimits(fvec& limits, const fvec& freqs, float trueSrate, float trueBlockSize, float maxSize);

void setParabolicWeights(fmatrix & pWeights, const fmatrix& pSignal, const cmatrix& sinusoids);

//if noise threshold exceeded;

fvec getSimilarity(const fmatrix& matrix, const fvec& signal);

fvec getPolarityWeights(const cmatrix& matrix, const fvec& signal);

fvec getPeaks(const fvec& signal);

void updateMonitor(fvec& monitor, const fvec& peaks, const fvec& countLimit);
void updateMonitor(fvec& monitor, const fvec& weights, int blocks);

int getOnOct(const fvec& octWeights, float threshold);

fvec getOnMidiNumbers(const fvec& midiMonitor, fvec& midiOn, const fvec& midiNumbers);
fvec getOffMidiNumbers(const fvec& midiMonitor, fvec& midiOn, const fvec& midiNumbers);

fvec getChanges(const fvec& turnOn, const fvec& turnOff);
int getMidiNumber(float freq, float refFreq);

void setXerr(fvec & xerr, int freqSize, float xmean);

float getXvar(const fvec& xerr);



//if noise threshold not exceeded;

//The rest~~~~~~~~~~~~~~~~~~~~~~~~`
bool propertiesChanged(const ProcessProperties& props, const PropertyHistory& past);

void saveProperties( ProcessProperties& props, PropertyHistory& past);


