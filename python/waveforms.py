import numpy as np

def time_input(time, srate):
    return np.arange(0, time, 1/srate)

def sample_input(samples, srate):
    time = samples/srate
    return np.arange(0, time, 1/srate)

def sinusoid(x, frequency, phase = 0, amplitude = 1):
    return amplitude * np.sin(2*np.pi*frequency*x  + phase*np.pi/180)

def sinusoid_t(time, srate, frequency, phase = 0, amplitude = 1):
    x = time_input(time, srate)
    return amplitude * np.sin(2*np.pi*frequency*x  + phase*np.pi/180)

def sinusoid_s(samples,srate, frequency, phase = 0, amplitude = 1):
    x = sample_input(samples, srate)
    return amplitude * np.sin(2*np.pi*frequency*x  + phase*np.pi/180)

def harmonic_signal_t(time, sample_rate, fundamental, peak = 1, phase = 0, harmonics=[1,2,3], weights = [10,10,1]):
    # harmonics assumes 1 = fundamental frequency
    x = time_input(time,sample_rate)
    y = np.zeros(x.shape)
    
    for i in range(min(len(harmonics), len(weights))):
        y = y + sinusoid(x, fundamental*harmonics[i],phase, weights[i])
    
    # proportionally scale signal so max value is equal to peak
    y = y*peak/np.max(np.abs(y) )
    
    return y

def harmonic_signal_s(samples, sample_rate, fundamental, peak = 1, phase = 0, harmonics=[1,2,3], weights = [10,10,1], sample_shift = 0):
    # harmonics assumes 1 = fundamental frequency
    x = sample_input(samples, sample_rate) + (sample_shift * 1/sample_rate)
    y = np.zeros(x.shape)
    
    for i in range(min(len(harmonics), len(weights))):
        y = y + sinusoid(x, fundamental*harmonics[i],phase, weights[i])
    
    # proportionally scale signal so max value is equal to peak
    y = y*peak/np.max(np.abs(y) )
    
    return y


def complex_sinusoid(x, frequency, phase = 0, amplitude = 1):
    omega = 2 * np.pi * frequency
    theta = phase*np.pi/180
    return amplitude * np.exp(1j*(omega*x + theta))

def complex_sinusoid_s(samples, srate, frequency, phase = 0, amplitude = 1):
    x = sample_input(samples, srate)
    return complex_sinusoid(x,frequency, phase, amplitude)

def complex_triangle_s(samples, srate, frequency):
    omega = 2 * np.pi * frequency
    theta = np.pi / 180
    x = sample_input(samples,srate)
    y = np.exp(1j*(omega*x + theta))
    y = np.arcsin(y)
    return y

#guitar mix
harmonics_long = [1,2,3,4,5,6,7]
weights_long = [1,.8,.66,.06,.03,.15,.11]
harmonics = [1,2,3,4,5,6,7]
weights = [1,.8,.66,.06,.03,.15,.11]
def complex_harmonic_signal_s(samples, srate, fundamental, peak = 1, phase = 0, harmonics=[1,2,3], weights = [10,10,1]):
    # harmonics assumes 1 = fundamental frequency
    y = np.zeros(samples, dtype = np.complex_)
    for i in range(min(len(harmonics), len(weights))):
        y = y + complex_sinusoid_s(samples,srate, fundamental*harmonics[i],phase, weights[i])
    
    # proportionally scale signal so max value is equal to peak
    #y = y*peak/np.max(np.abs(y) )
    
    return y
