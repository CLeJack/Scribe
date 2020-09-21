"""
Custom Fourier Transform

A variation on the idea of the discrete fourier transform

"""
import numpy as np
import waveforms as W

def cmatrix(samples, srate, freqs, phase =0 ):

    waves = []

    for f in freqs:
        waves.append(W.complex_sinusoid_s(samples, srate,f, phase))

    return np.array(waves)

def dct(cmatrix,signal, **kwargs):
    output = dct_coeff(cmatrix, signal, **kwargs)
    output = dct_amp(output, signal)
    return output

def dct_coeff(cmatrix,signal, **kwargs):
    #i = initial, f = final
    rowi = kwargs.get('rowi', 0)
    rowf = kwargs.get('rowf',cmatrix.shape[0])
    indi = kwargs.get('indi', 0)
    indf = kwargs.get('indf', len(signal))

    sig = signal[indi:indf]
    mat = cmatrix[ rowi:rowf, indi:indf]
    output = np.dot(mat, sig)
    return output

def dct_amp(coeff, signal):
    return np.abs(coeff) /len(signal)

def dct_concentration(dctamp, octsize = 12):
    maxind = 0
    total = 0
    output =  np.zeros(dctamp.shape)
    truemax = np.argmax(dctamp) - 2 * octsize

    for i in range(1, len(dctamp)-1):
        if (dctamp[i-1] < dctamp[i] and dctamp[i] > dctamp[i+1]) :
            maxind = i

        if dctamp[i-1] > dctamp[i] and dctamp[i] < dctamp[i+1] and i >= truemax:
            output[maxind] = total
            total = 0
            maxind = 0
        else:
            total += dctamp[i]

    return output



# experimental stuff~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def trimatrix(samples, srate, freqs):

    """

    the behavior of a complex sinusoid with trigonometric functions
    gives undesirable results. The imaginary and real components
    need to be dealt with separately.

    consequently, it's worth exploring harmonic sinusoids again.
    """
    mat = cmatrix(samples,srate, freqs)
    for row in range(mat.shape[0]):
        mat[row].real = (2/np.pi) * np.arcsin(mat[row].real)
        mat[row].imag = (2/np.pi) * np.arcsin(mat[row].imag)

    return mat

def paramatrix(samples, srate, freqs):
    mat = cmatrix(samples, srate, freqs)

    for row in range(mat.shape[0]):
        mat[row].real = ((2 / np.pi) * np.arcsin(mat[row].real))**2
        mat[row].imag = ((2 / np.pi) * np.arcsin(mat[row].imag))**2

    return mat

def harmonic_matrix(samples,srate,freqs, harmonics = [1,2,3], weights = [1,.8,.6]):

    waves = []

    for f in freqs:
        waves.append(W.complex_harmonic_signal_s(samples,srate,f,peak=1,
                                                 phase = 0, harmonics = harmonics,
                                                 weights = weights))

    return np.array(waves)

def expand_frequencies(freqs):

    output = np.zeros( (len(freqs)-1)*3 +1)

    for i in range(len(freqs)- 1):
        ind = 3*i
        output[ind] = freqs[i]

        interval = (freqs[i+1] - freqs[i])/3
        output[ind+1] = output[ind] + interval
        output[ind+2] = output[ind] + 2 * interval

    output[len(output) -1] = freqs[len(freqs) - 1]

    return output


def get_peaks(arr):
    # naive peak finding for a smooth signal
    # meant to be used specifically with the results of cft.matrix
    output = np.zeros(arr.shape[0])
    for i in range(1,arr.shape[0]-1):
        output[i] = 1 if arr[i-1] < arr[i] > arr[i+1] else 0

    if arr[0] > arr[1]:
        output[0] =1

    end = len(arr) -1
    if arr[end] < arr[end - 1]:
        output[end] = 0

    #this method also assumes a smooth signal
    return output

# parabolic kernel~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~``
def oddWindow(originalSize):
    return 2 * int(originalSize/2) + 1

def frequencyWindow(freq, srate):
    return oddWindow((srate/freq))

def parabola(window):
    xs = np.linspace(-1,1, window)
    ys = xs* xs
    ys = 2 * ys/np.sum(ys)
    return ys[:,np.newaxis]



