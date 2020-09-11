import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import scipy.signal as S
import scipy.io.wavfile as wavfile
import os
from functools import reduce

import __python.matrices as M
import __python.waveforms as W
import __python.tuning as T


def get_signal(path):
    return wavfile.read(path)[1]

def signal_to_csv(path):
    sound = get_signal(path)
    df = pd.DataFrame(sound)
    name = path.split('.')[0]
    df.to_csv(name + ".csv", index = False)


def get_comy(arr):
    indices = np.arange(arr.shape[0])

    numerator = np.sum(np.abs(arr) * indices)
    denominator = np.sum(indices)

    return numerator/denominator

def get_comy2(arr):
    indices = np.arange(arr.shape[0])

    imid = int(arr.shape[0]/2)
    iend = arr.shape[0]
    numerator1 = np.sum(np.abs(arr[:imid]) * indices[:imid])
    denominator1 = np.sum(indices[:imid])

    numerator2 = np.sum(np.abs(arr[imid:iend]) * indices[imid:iend])
    denominator2 = np.sum(indices[imid:iend])

    out1 = (numerator1 + numerator2)/(denominator1 + denominator2)
    out2 = numerator2/denominator2

    return [out1,out2]


def get_comx(arr):
    indices = np.arange(arr.shape[0])
    abs_arr = np.abs(arr)

    numerator = np.sum(abs_arr * indices)
    denominator = np.sum(abs_arr)

    return numerator/denominator

def get_comx2(arr):
    indices = np.arange(arr.shape[0])

    abs_arr = np.abs(arr)
    imid = int(arr.shape[0]/2)
    iend = arr.shape[0]

    numerator1 = np.sum(abs_arr[:imid] * indices[:imid])
    denominator1 = np.sum(abs_arr[:imid])

    numerator2 = np.sum(abs_arr[imid:iend] * indices[imid:iend])
    denominator2 = np.sum(abs_arr[imid:iend])

    out1 = (numerator1 + numerator2)/(denominator1 + denominator2)
    out2 = numerator2/denominator2

    return [out1,out2]


def sum_norm(arr):
    return arr/np.sum(arr)

def rms(arr, **kwargs):
    indi = kwargs.get("indi", 0)
    indf = kwargs.get("indf", arr.shape[0])

    a = arr[indi:indf]
    return np.sqrt(np.sum(a*a)/len(a))

def weightRatio(weights, octSize = 12):
    output = np.zeros(weights.shape)
    for i in range(octSize, weights.shape[0]):
        output[i] = weights[i]/weights[i-octSize]
    return output

def octaveMedians(ratios, octSize = 12):
    octaves = int(ratios.shape[0]/octSize)
    output = np.zeros(octaves);
    for i in range(octaves):
        output[i] = np.median(ratios[i*octSize:(i*octSize + octSize)])

    return output;

def wav_to_csv(indir = '__wav/', outdir = '__wavcsv/'):
    files = os.listdir(indir)
    files = list(filter(lambda x: ".wav" in x, files))
    dfs = [pd.DataFrame(get_signal(indir + x)) for x in files]

    for file, df in zip(files, dfs):
        df.to_csv(outdir + file.split(".")[0] + ".csv", index = False)


def load_csvs(indir = '__wavcsv/', use_filename = True):
    arrs = []
    files = os.listdir(indir)
    files = list(filter(lambda x: ".csv" in x, files))

    for f in files:
        if use_filename:
            arrs.append(pd.read_csv(indir + f, names = [f.split(".")[0]] ))
        else:
            arrs.append(pd.read_csv(indir + f))

    return arrs

class PData():
    srate = 44100
    windowtime = .05
    block = 128

    ds_factor = 10
    ds_window = int(srate * windowtime // ds_factor)
    ds_rate = srate//ds_factor
    ds_block = block // ds_factor

    ref_freq = 440
    semitone = 2 ** (1 / 12)
    lowExp = -57  # C0
    highExp = 50 - 24  # B8

    freqs = T.get_freqs(ref_freq, semitone, lowExp, highExp)
    matrix = M.cmatrix(ds_window,ds_rate,freqs)


    def __init__(self, srate =44100, windowtime = .05, block = 128, ds_factor = 10):
        PData.srate = srate
        PData.windowtime = windowtime
        PData.block = block

        PData.ds_factor = ds_factor
        PData.ds_rate = srate//ds_factor
        PData.ds_window = srate * windowtime // ds_factor
        PData.ds_block = block // ds_factor
        PData.freqs = T.get_freqs(PData.ref_freq, PData.semitone, PData.lowExp, PData.highExp)
        PData.matrix = M.cmatrix(PData.ds_window, PData.srate, PData.freqs)


def process(signal, opath="__results/" ):

    ds_factor = PData.ds_factor
    ds_window = PData.ds_window
    ds_block = PData.ds_block

    name = signal.columns[0]

    ds_signal = signal[::ds_factor].to_numpy().flatten()

    windows = int((ds_signal.shape[0] - ds_window)//ds_block)
    matrix = PData.matrix
    freqs = PData.freqs

    indi = ds_window - ds_window//2
    indf = ds_window

    header = ['comy', 'f0ind', 'note', 'trigger', 'retrigger', 'octmetric', 'comw']
    valuedata = []
    weightdata = []
    octthresh = 4

    for i in range(windows):
        start = i*ds_block
        end = start + ds_window
        signal = ds_signal[start:end]

        weights = M.dct(matrix, signal, indi=indi, indf=indf)
        weights = sum_norm(weights)
        ratios = weightRatio(weights)

        f0ind = np.argmax(weights)
        note = f0ind
        if ratios[f0ind] < octthresh:
                note = f0ind - 12

        comy2 = get_comy2(signal)
        trigger = np.max(weights)
        retrigger = comy2[1]/comy2[0]
        comy = comy2[0]
        comw = get_comx(weights)

        valuedata.append([comy, f0ind, note, trigger, retrigger, ratios[f0ind], comw])

        weightdata.append(weights.flatten())

    valuedata = np.array(valuedata)
    weightdata = np.array(weightdata)

    vdf = pd.DataFrame(valuedata, columns=header)
    wdf = pd.DataFrame(weightdata, columns=freqs.astype(int))

    p1 = opath + name + "-values.csv"
    p2 = opath + name + "-weights.csv"

    vdf.to_csv(p1, index=False)
    wdf.to_csv(p2, index=False)


def save_results(indir="__wavcsv/", outdir="__results/"):
    dfs = load_csvs(indir)

    for df in dfs:
        process(df, outdir)

def deharm(data, start_ind, samples, freq, srate, ds_factor = 10):
    data = data[::ds_factor].flatten()
    window = M.frequencyWindow(freq,srate)
    kernel = M.parabola(window).flatten()
    test = data[start_ind : start_ind + samples].flatten()
    cmat = M.cmatrix(samples, srate,PData.freqs)

    result = S.convolve(test, kernel, mode="same").flatten()

    test_spectrum = M.dct(cmat, test )
    result_spectrum = M.dct(cmat, result)

    output = {'test':test, 'result':result, 'tspectrum':test_spectrum, 'rspectrum':result_spectrum}

    return output



