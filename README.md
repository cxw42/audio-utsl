# audio-utsl

A C audio player library you can build from source!
Integrates libsndfile with portaudio.

This project is intended to fill the same niche that BASS does.  BASS is
awesome, but I couldn't get it to run recently, possibly because of
a false positive on the AV scan.  This project doesn't require any binary
blobs.

## Installation

Install libsndfile, portaudio, and pthreads.  E.g., on cygwin, those are
packages you can install from setup.exe.

## Internals

 - A producer thread to read from the source using libsndfile
 - A portaudio callback consumer (which probably runs in portaudio's own thread)
   to pass the data to portaudio
 - [PortAudio's ring buffer](https://app.assembla.com/spaces/portaudio/git/source/master/src/common/pa_ringbuffer.h)
   to pass ownership of blocks of data between the producer and the consumer

## Links

 - <http://www.rossbencina.com/code/lockfree>
 - <https://www.cs.cmu.edu/~410-s05/lectures/L31_LockFree.pdf>
