### Scribe

Scribe is a monophonic, open source, midi conversion VST built with JUCE.

The Windows and Linux versions of this VST are hosted at: 

limitegauge.com

I currently do not have a Mac system to compile and test the Mac build with, but the source can be downloaded and compiled using the Mac JUCE exporter.

#### Quick Start and Tips

* Use a sample rate 44100 or 48000. Other sample rates may work, but they have not been tested.

* Use the neck pickup on your guitar, and turn the tone knob all the way down to reduce harmonic
content. If the frequency spectrum plot shows more than two peaks for a single note, there may be too
much harmonic content for accurate detection.

* For percussion and other fast attack instruments, if your picking speed produces slight errors, try
playing above the 9th fret, and using the midi panel to octave shift to your desired octave.

* Don’t forget to tune your guitar. This is especially important for the low E string at the 5th fret and below.

* Have Fun!


### How it works

Scribe has a very simple design. The primary logic that makes it work is contained in the DCT.h and DCT.cpp files. 
DCT stands for Discrete Customized Transform--borrowing from the Discrete Fourier Transform.

The Fourier Transform and its derivatives are bound by sample rate and an analysis window which give a fixed frequency resolution.
As a consequence, to get a resolution of one hertz with something like a DFT or FFT would require a window of 44100 samples.

Scribe takes advantage of the following facts.
* There is no intention of modifying or reconstructing the original signal, so negative frequencies aren't needed.
* The analysis window only needs to be large enough to contain a single cycle of the lowest frequency of interest.
* The range of a guitar is small, so the signal can be downsampled dramatically.
* Other FFT algorithms attempt to give insight into a large range of the frequency spectrum, but there's nothing stopping us from choosing only frequencies of interests. In the case of Scribe, this means 12TET frequencies with a 440 hz reference.


#### License

Scribe has an MIT license. The following use of "software" refers to Scribe and any of it's components.


Copyright 2020 Charles Jackson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
