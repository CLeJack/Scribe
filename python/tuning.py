import numpy as np



def get_freqs(ref_freq, semitone, low_exp, high_exp):
    # freq = fundamental * SEMITONE^exponent
    return np.array([ref_freq * semitone ** n for n in range(low_exp, high_exp + 1)])

def calculate_linear_exnotes(self):
    count = self.frequencies.shape[0]

    interval = self.frequencies.copy()
    interval = (np.roll(self.frequencies,-1) - interval)/3
    o1 = self.frequencies.copy()
    o2 = o1 + interval
    o3 = o2 + interval

    output = np.array([o1, o2, o3]).transpose().reshape(count*3,1).flatten()
    return output[:output.shape[0]-2]

def calculate_nonlinear_exnotes(self):
    count = self.frequencies.shape[0]
    low = self.C0_exponent
    high = self.B8_exponent

    o1 = self.frequencies.copy()
    o2 = np.array([self.tuning_freq * self.semitone ** (n + 1/3) for n in range(low, high + 1)])
    o3 = np.array([self.tuning_freq * self.semitone ** (n + 2/3) for n in range(low, high + 1)])

    output = np.array([o1, o2, o3]).transpose().reshape(count * 3, 1).flatten()
    return output[:output.shape[0] - 2]

def get_index(freq, refFreq, semitone, minExponent):
    # the lowest note of consideration (i.e. C0_exponent) will be considered 0
    # this can then be used to lookup values from the note table by note name of frequency

    # reverse calculate_notes()
    semitone_to_exp = freq / refFreq

    exponent = np.log(semitone_to_exp) / np.log(semitone)

    # can't go below the lowest exponent / C0
    exponent = max(minExponent, exponent)
    offset = int(0.5 + exponent - minExponent)  # round up to the nearest exponent
    return offset

# integer based pitch class
def get_class_index(freq):

    offset = get_index(freq)
    index = offset % 12
    return index

def get_octave(freq):
    offset = get_index(freq)
    octave = offset//12
    return octave

def oct_ind_to_freq(freqs, octave, index):
    i = octave*12 + index
    return freqs[i]

def crush(exarr, final_size, shift = 1, factor = 3):
    output = np.zeros(final_size)

    for exind in range(exarr):
        ind = (exind + shift)//factor
        output[ind] = np.max(output[ind], exarr[exind])
